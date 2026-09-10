#include "Player/SHHealthComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

USHHealthComponent::USHHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USHHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USHHealthComponent::HandleTakeAnyDamage);
	}
}

void USHHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHHealthComponent, CurrentHealth);
	DOREPLIFETIME(USHHealthComponent, bIsStaggered);
}

void USHHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Damage <= 0.f || bIsStaggered)
	{
		return;
	}

	CurrentHealth = FMath::Max(0.f, CurrentHealth - Damage);
	OnRep_CurrentHealth();

	if (CurrentHealth <= 0.f)
	{
		Server_BeginStagger(DamageCauser);
	}
}

void USHHealthComponent::Server_BeginStagger(AActor* DamageCauser)
{
	bIsStaggered = true;
	OnRep_IsStaggered();

	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		FVector KnockbackDir = OwnerCharacter->GetActorLocation();
		if (DamageCauser)
		{
			KnockbackDir -= DamageCauser->GetActorLocation();
		}
		KnockbackDir.Z = 0.f;
		KnockbackDir = KnockbackDir.IsNearlyZero() ? -OwnerCharacter->GetActorForwardVector() : KnockbackDir.GetSafeNormal();

		const FVector LaunchVelocity = KnockbackDir * KnockbackStrength + FVector(0.f, 0.f, KnockbackUpwardStrength);
		OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}

	GetWorld()->GetTimerManager().SetTimer(StaggerTimerHandle, this, &USHHealthComponent::Server_EndStagger, StaggerDurationSeconds, false);
}

void USHHealthComponent::Server_EndStagger()
{
	bIsStaggered = false;
	CurrentHealth = MaxHealth;
	OnRep_IsStaggered();
	OnRep_CurrentHealth();
}

void USHHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(GetNormalizedHealth());
}

void USHHealthComponent::OnRep_IsStaggered()
{
	if (bIsStaggered)
	{
		OnStaggered.Broadcast();
	}
	else
	{
		OnRecovered.Broadcast();
	}
}
