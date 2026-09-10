#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SHTypes.h"
#include "BTDecorator_IsInState.generated.h"

/**
 * Generic reusable state-gate decorator (Section 9) - reads the Resident's CurrentState
 * directly (not the Blackboard mirror, so it's correct even between service ticks) and
 * passes if it matches RequiredState. Used throughout BT_Resident instead of one decorator
 * per state (IsSuspicious/HasDetectedPlayer/etc all become this + a different RequiredState).
 */
UCLASS()
class SUBURBANHEIST_API UBTDecorator_IsInState : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsInState();

	UPROPERTY(EditAnywhere, Category = "Resident")
	EResidentState RequiredState = EResidentState::Idle;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
