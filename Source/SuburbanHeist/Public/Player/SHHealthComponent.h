#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnHealthChanged, float, NormalizedHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnStaggered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnRecovered);

/**
 * Section 18's "player health" requirement, sized for this game's tone: getting caught by a
 * Resident's fictional chase attack (Section 11) staggers the player - a comedic stumble with
 * knockback, not a kill. There's no permadeath and nothing carried is lost (money is banked to
 * the team the instant an objective completes, per Section 13), matching Section 1's chaotic
 * party-game framing rather than a realistic combat system. Attach to ASHPlayerCharacter.
 */
UCLASS(ClassGroup = (SuburbanHeist), meta = (BlueprintSpawnableComponent))
class SUBURBANHEIST_API USHHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USHHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	/** How long the stagger (knocked down, can't sprint/interact - enforce in Blueprint via
	 *  IsStaggered()) lasts before automatically recovering to full health. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float StaggerDurationSeconds = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float KnockbackStrength = 800.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float KnockbackUpwardStrength = 300.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_IsStaggered, BlueprintReadOnly, Category = "Health")
	bool bIsStaggered = false;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FSHOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FSHOnStaggered OnStaggered;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FSHOnRecovered OnRecovered;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetNormalizedHealth() const { return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsStaggered() const { return bIsStaggered; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION() void OnRep_CurrentHealth();
	UFUNCTION() void OnRep_IsStaggered();

	void Server_BeginStagger(AActor* DamageCauser);
	void Server_EndStagger();

private:
	FTimerHandle StaggerTimerHandle;
};
