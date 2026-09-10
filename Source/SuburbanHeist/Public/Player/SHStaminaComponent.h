#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHStaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnStaminaChanged, float, NormalizedStamina);

/**
 * Simple sprint-gated stamina system (Section 16). Ticks only while sprinting or
 * regenerating below max, to avoid a permanent per-frame cost (Section 27).
 */
UCLASS(ClassGroup = (SuburbanHeist), meta = (BlueprintSpawnableComponent))
class SUBURBANHEIST_API USHStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USHStaminaComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float DrainPerSecond = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float RegenPerSecond = 12.f;

	/** Stamina must recover to at least this fraction before sprint is allowed again once exhausted. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "1"))
	float ExhaustedRecoveryFraction = 0.25f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100.f;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FSHOnStaminaChanged OnStaminaChanged;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetSprinting(bool bSprinting);

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool CanSprint() const;

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetNormalizedStamina() const { return MaxStamina > 0.f ? CurrentStamina / MaxStamina : 0.f; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_CurrentStamina();

private:
	bool bWantsSprint = false;
	bool bIsExhausted = false;
};
