#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToSuspicionLocation.generated.h"

/**
 * Investigating/Chase movement (Section 9): moves toward Blackboard TargetActor if set,
 * otherwise LastHeardLocation. Used for both the Investigating branch (walk to noise) and
 * the Chase branch (pursue target) - AcceptableRadius/bUseTargetActor differ per BT node
 * instance so the same task class covers both.
 */
UCLASS()
class SUBURBANHEIST_API UBTTask_MoveToSuspicionLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToSuspicionLocation();

	UPROPERTY(EditAnywhere, Category = "Resident")
	float AcceptableRadius = 80.f;

	/** If true, moves toward the live TargetActor (Chase); if false, moves toward the last
	 *  known LastHeardLocation snapshot (Investigating). */
	UPROPERTY(EditAnywhere, Category = "Resident")
	bool bUseTargetActor = false;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
