#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SHResidentAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
struct FAIStimulus;

/**
 * Drives one Resident's perception (Section 10) and runs the shared BT_Resident behavior
 * tree. Perception config (radius/angle/hearing threshold) is pulled from the possessed
 * ASHResidentCharacter's archetype tuning so archetypes genuinely behave differently
 * (Section 8) from one shared AIController class.
 */
UCLASS()
class SUBURBANHEIST_API ASHResidentAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASHResidentAIController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UFUNCTION(BlueprintCallable, Category = "AI|Debug")
	void Debug_SetAIPaused(bool bPaused);

	UFUNCTION(BlueprintPure, Category = "AI|Debug")
	bool IsAIPaused() const { return bAIPaused; }

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	bool bAIPaused = false;
};
