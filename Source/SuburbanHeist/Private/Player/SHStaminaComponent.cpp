#include "Player/SHStaminaComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

USHStaminaComponent::USHStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Disabled by default - only enabled on demand so we're not ticking a healthy, idle player.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void USHStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
}

void USHStaminaComponent::SetSprinting(bool bSprinting)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bWantsSprint = bSprinting && CanSprint();
	const bool bNeedsTick = bWantsSprint || CurrentStamina < MaxStamina;
	SetComponentTickEnabled(bNeedsTick);
}

bool USHStaminaComponent::CanSprint() const
{
	if (bIsExhausted)
	{
		return CurrentStamina >= MaxStamina * ExhaustedRecoveryFraction;
	}
	return CurrentStamina > 0.f;
}

void USHStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	const float Previous = CurrentStamina;

	if (bWantsSprint && CanSprint())
	{
		CurrentStamina = FMath::Max(0.f, CurrentStamina - DrainPerSecond * DeltaTime);
		if (CurrentStamina <= 0.f)
		{
			bIsExhausted = true;
			bWantsSprint = false;
		}
	}
	else
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + RegenPerSecond * DeltaTime);
		if (bIsExhausted && CurrentStamina >= MaxStamina * ExhaustedRecoveryFraction)
		{
			bIsExhausted = false;
		}
	}

	if (!FMath::IsNearlyEqual(Previous, CurrentStamina))
	{
		OnRep_CurrentStamina();
	}

	if (!bWantsSprint && CurrentStamina >= MaxStamina)
	{
		SetComponentTickEnabled(false);
	}
}

void USHStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHStaminaComponent, CurrentStamina);
}

void USHStaminaComponent::OnRep_CurrentStamina()
{
	OnStaminaChanged.Broadcast(GetNormalizedStamina());
}
