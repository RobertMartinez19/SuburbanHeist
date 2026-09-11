#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SHTypes.h"
#include "Interaction/SHInteractableInterface.h"
#include "SHObjectivePoint.generated.h"

class UDataTable;
class UStaticMeshComponent;
class USphereComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnObjectiveCompletedLocal);

/**
 * BP_ObjectivePoint (Section 12/13). Fictional, abstract objective - never implements real
 * lock/security bypass logic, only an arcade hold-to-interact (or designer-driven QTE / key
 * item, via the BlueprintImplementableEvent hooks below). Rolls its own dollar value from
 * FSHObjectiveDataRow on spawn so every match's loot placement/value feels different
 * (Section 23) while staying data-driven (Section 26).
 */
UCLASS()
class SUBURBANHEIST_API ASHObjectivePoint : public AActor, public ISHInteractableInterface
{
	GENERATED_BODY()

public:
	ASHObjectivePoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ObjectiveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	EObjectiveCategory Category = EObjectiveCategory::ValuableObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	TObjectPtr<UDataTable> ObjectiveDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FName DataRowName;

	UPROPERTY(ReplicatedUsing = OnRep_Completed, BlueprintReadOnly, Category = "Objective")
	bool bCompleted = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Objective")
	float RolledValue = 0.f;

	UPROPERTY(BlueprintAssignable, Category = "Objective|Events")
	FSHOnObjectiveCompletedLocal OnCompletedLocal;

	UFUNCTION(BlueprintPure, Category = "Objective")
	bool IsCompleted() const { return bCompleted; }

	/** Section 24 CompleteObjective debug command. */
	void Debug_ForceComplete();

	/** Re-rolls value and reopens the objective for a new match (Section 31 acceptance test
	 *  #18 "complete another match"). Called by ASHGameMode::Server_RestartMatch. */
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void Server_ResetForRematch();

	// -- ISHInteractableInterface --
	// (Parameter named InteractingCharacter, not Instigator - AActor already has an
	// Instigator member, and reusing that name on an AActor-derived override shadows it.)
	virtual void Interact_Implementation(ACharacter* InteractingCharacter) override;
	virtual bool CanInteract_Implementation(ACharacter* InteractingCharacter) const override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual float GetInteractionDuration_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_Completed();

	bool GetTuning(FSHObjectiveDataRow& OutRow) const;

	/** Optional per-second noise while interacting (high-risk objectives), driven by the
	 *  interaction component's progress rather than a Tick on this actor (Section 27). */
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void Server_EmitInteractionTickNoise();
};
