#include "Gameplay/SHObjectivePoint.h"
#include "Gameplay/SHLootManager.h"
#include "Gameplay/SHNoiseManager.h"
#include "SHGameState.h"
#include "SHPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ASHObjectivePoint::ASHObjectivePoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->InitSphereRadius(60.f);
	InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(InteractionVolume);

	ObjectiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	ObjectiveMesh->SetupAttachment(RootComponent);
	ObjectiveMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ASHObjectivePoint::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FSHObjectiveDataRow Row;
		if (GetTuning(Row))
		{
			// Random per-match value (Section 23 randomization, Section 13 value bands).
			RolledValue = FMath::RandRange(Row.MinValue, Row.MaxValue);
		}
	}
}

bool ASHObjectivePoint::GetTuning(FSHObjectiveDataRow& OutRow) const
{
	if (!ObjectiveDataTable || DataRowName.IsNone())
	{
		return false;
	}
	static const FString Context(TEXT("ASHObjectivePoint::GetTuning"));
	if (const FSHObjectiveDataRow* Found = ObjectiveDataTable->FindRow<FSHObjectiveDataRow>(DataRowName, Context))
	{
		OutRow = *Found;
		return true;
	}
	return false;
}

void ASHObjectivePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASHObjectivePoint, bCompleted);
	DOREPLIFETIME(ASHObjectivePoint, RolledValue);
}

void ASHObjectivePoint::OnRep_Completed()
{
	OnCompletedLocal.Broadcast();
}

bool ASHObjectivePoint::CanInteract_Implementation(ACharacter* Instigator) const
{
	return !bCompleted;
}

FText ASHObjectivePoint::GetInteractionPrompt_Implementation() const
{
	switch (Category)
	{
		case EObjectiveCategory::ValuableObject:      return NSLOCTEXT("SuburbanHeist", "PromptSearch", "[E] SEARCH");
		case EObjectiveCategory::HiddenCache:         return NSLOCTEXT("SuburbanHeist", "PromptSearch2", "[E] SEARCH");
		case EObjectiveCategory::SafeObjective:       return NSLOCTEXT("SuburbanHeist", "PromptOpen", "[E] OPEN");
		case EObjectiveCategory::ElectronicsObjective:return NSLOCTEXT("SuburbanHeist", "PromptCollect", "[E] COLLECT");
		case EObjectiveCategory::Collectible:         return NSLOCTEXT("SuburbanHeist", "PromptCollect2", "[E] COLLECT");
		case EObjectiveCategory::BonusObjective:      return NSLOCTEXT("SuburbanHeist", "PromptInteract", "[E] INTERACT");
		default:                                      return NSLOCTEXT("SuburbanHeist", "PromptInteractDefault", "[E] INTERACT");
	}
}

float ASHObjectivePoint::GetInteractionDuration_Implementation() const
{
	FSHObjectiveDataRow Row;
	return GetTuning(Row) ? Row.InteractionDurationSeconds : 3.f;
}

void ASHObjectivePoint::Interact_Implementation(ACharacter* Instigator)
{
	if (!HasAuthority() || bCompleted)
	{
		return;
	}

	bCompleted = true;
	OnRep_Completed();

	FSHObjectiveDataRow Row;
	const bool bHasRow = GetTuning(Row);
	const float NoiseOnComplete = bHasRow ? Row.NoiseOnComplete : 15.f;

	TArray<AActor*> NoiseManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), NoiseManagers);
	if (NoiseManagers.Num() > 0)
	{
		if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(NoiseManagers[0]))
		{
			FSHNoiseEvent Event;
			Event.Location = GetActorLocation();
			Event.Loudness = NoiseOnComplete;
			Event.Radius = 800.f;
			Event.SourceType = ENoiseSourceType::Other;
			Event.Instigator = this;
			Noise->Server_ReportNoise(Event);
		}
	}

	TArray<AActor*> LootManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHLootManager::StaticClass(), LootManagers);
	if (LootManagers.Num() > 0)
	{
		if (ASHLootManager* Loot = Cast<ASHLootManager>(LootManagers[0]))
		{
			Loot->Server_NotifyObjectiveCompleted(this, RolledValue, Instigator ? Instigator->GetPlayerState() : nullptr);
		}
	}
}

void ASHObjectivePoint::Server_EmitInteractionTickNoise()
{
	if (!HasAuthority())
	{
		return;
	}

	FSHObjectiveDataRow Row;
	if (!GetTuning(Row) || Row.NoisePerSecondWhileInteracting <= 0.f)
	{
		return;
	}

	TArray<AActor*> NoiseManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), NoiseManagers);
	if (NoiseManagers.Num() > 0)
	{
		if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(NoiseManagers[0]))
		{
			FSHNoiseEvent Event;
			Event.Location = GetActorLocation();
			Event.Loudness = Row.NoisePerSecondWhileInteracting;
			Event.Radius = 800.f;
			Event.SourceType = ENoiseSourceType::Other;
			Event.Instigator = this;
			Noise->Server_ReportNoise(Event);
		}
	}
}

void ASHObjectivePoint::Debug_ForceComplete()
{
	Interact_Implementation(nullptr);
}
