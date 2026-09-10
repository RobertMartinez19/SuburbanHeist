#include "AI/SHResidentCharacter.h"
#include "Gameplay/SHTeamDetectionManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

ASHResidentCharacter::ASHResidentCharacter()
{
	PrimaryActorTick.bCanEverTick = false; // driven by BTService ticks, not a per-frame Tick (Section 27)
	bReplicates = true;
}

void ASHResidentCharacter::BeginPlay()
{
	Super::BeginPlay();
	HomeLocation = GetActorLocation();
	LoadTuning();
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = Tuning.WalkSpeed;
	}
}

void ASHResidentCharacter::LoadTuning()
{
	if (!ArchetypeDataTable)
	{
		return;
	}

	FName RowName = ArchetypeRowNameOverride;
	if (RowName.IsNone())
	{
		RowName = FName(*UEnum::GetValueAsString(Archetype).RightChop(FString(TEXT("EResidentArchetype::")).Len()));
	}

	static const FString Context(TEXT("ASHResidentCharacter::LoadTuning"));
	if (const FSHResidentArchetypeDataRow* Row = ArchetypeDataTable->FindRow<FSHResidentArchetypeDataRow>(RowName, Context))
	{
		Tuning = *Row;
	}
}

void ASHResidentCharacter::SetArchetypeByName(const FString& ArchetypeName)
{
	static const UEnum* EnumPtr = StaticEnum<EResidentArchetype>();
	const int64 Value = EnumPtr->GetValueByNameString(ArchetypeName);
	if (Value != INDEX_NONE)
	{
		Archetype = static_cast<EResidentArchetype>(Value);
	}
	LoadTuning();
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = Tuning.WalkSpeed;
	}
}

void ASHResidentCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASHResidentCharacter, CurrentState);
	DOREPLIFETIME(ASHResidentCharacter, SuspicionLevel);
}

void ASHResidentCharacter::OnRep_CurrentState()
{
	// Broadcast handled by caller via Server_SetState on the server; OnRep fires this on clients.
}

void ASHResidentCharacter::OnRep_SuspicionLevel()
{
}

void ASHResidentCharacter::Server_AddSuspicion(float Delta, AActor* SourceActor, FVector SourceLocation)
{
	if (!HasAuthority() || CurrentState == EResidentState::Detected || CurrentState == EResidentState::Chase)
	{
		return;
	}

	SuspicionLevel = FMath::Clamp(SuspicionLevel + Delta, 0.f, 100.f);

	if (SourceActor)
	{
		TargetActor = SourceActor;
	}
	LastHeardLocation = SourceLocation;

	// Evaluate transitions inline (see header comment: Character owns the whole FSM).
	if (SuspicionLevel >= Tuning.DetectionThreshold)
	{
		Server_SetState(EResidentState::Detected);
	}
	else if (SuspicionLevel >= Tuning.InvestigateThreshold)
	{
		if (CurrentState != EResidentState::Investigating && CurrentState != EResidentState::Alert)
		{
			Server_SetState(EResidentState::Investigating);
		}
		if (SuspicionLevel >= (Tuning.InvestigateThreshold + Tuning.DetectionThreshold) * 0.5f)
		{
			Server_SetState(EResidentState::Alert);
		}
	}
	else if (SuspicionLevel > 0.f && CurrentState == EResidentState::Idle)
	{
		Server_SetState(EResidentState::Suspicious);
	}
}

void ASHResidentCharacter::Server_TickSuspicionDecay(float DeltaSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	if (CurrentState == EResidentState::Chase)
	{
		ChaseElapsedSeconds += DeltaSeconds;
		if (ChaseElapsedSeconds >= Tuning.MaxChaseDurationSeconds)
		{
			Server_SetState(EResidentState::Return);
		}
		return;
	}

	if (CurrentState == EResidentState::Detected || CurrentState == EResidentState::Return)
	{
		return;
	}

	SuspicionLevel = FMath::Max(0.f, SuspicionLevel - Tuning.SuspicionDecayPerSecond * DeltaSeconds);

	if (SuspicionLevel <= 0.f && (CurrentState == EResidentState::Suspicious || CurrentState == EResidentState::Investigating || CurrentState == EResidentState::Alert))
	{
		Server_SetState(EResidentState::Idle);
	}
}

void ASHResidentCharacter::Server_SetState(EResidentState NewState)
{
	if (!HasAuthority() || NewState == CurrentState)
	{
		return;
	}

	const EResidentState OldState = CurrentState;
	CurrentState = NewState;
	ApplyMovementSpeedForState(NewState);
	OnRep_CurrentState();
	OnStateChanged.Broadcast(OldState, NewState);

	if (NewState == EResidentState::Detected)
	{
		TArray<AActor*> Managers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHTeamDetectionManager::StaticClass(), Managers);
		if (Managers.Num() > 0)
		{
			if (ASHTeamDetectionManager* DM = Cast<ASHTeamDetectionManager>(Managers[0]))
			{
				DM->Server_ReportDetection(this, TargetActor.Get());
			}
		}
		ChaseElapsedSeconds = 0.f;
		Server_SetState(EResidentState::Chase);
	}
	else if (NewState == EResidentState::Idle)
	{
		SuspicionLevel = 0.f;
		TargetActor = nullptr;
	}
}

void ASHResidentCharacter::ApplyMovementSpeedForState(EResidentState State)
{
	if (!GetCharacterMovement())
	{
		return;
	}

	switch (State)
	{
		case EResidentState::Investigating:
			GetCharacterMovement()->MaxWalkSpeed = Tuning.InvestigateSpeed;
			break;
		case EResidentState::Chase:
			GetCharacterMovement()->MaxWalkSpeed = Tuning.ChaseSpeed;
			break;
		default:
			GetCharacterMovement()->MaxWalkSpeed = Tuning.WalkSpeed;
			break;
	}
}

void ASHResidentCharacter::Server_PerformChaseAttack(AActor* Target)
{
	if (!HasAuthority() || !Target || !CanAttack())
	{
		return;
	}

	// Abstract, fictional effect only (Section 11) - no real weapon/ammo/ballistics simulation.
	// Wire this into your project's health/knockback system; left minimal here on purpose.
	UGameplayStatics::ApplyDamage(Target, ChaseAttackDamage, GetController(), this, nullptr);
}

void ASHResidentCharacter::Server_MarkAttackAttempted()
{
	if (HasAuthority())
	{
		LastAttackWorldTime = GetWorld()->GetTimeSeconds();
	}
}

void ASHResidentCharacter::Server_BeginAttackTelegraph(AActor* Target)
{
	if (HasAuthority())
	{
		OnAttackTelegraph(Target);
	}
}
