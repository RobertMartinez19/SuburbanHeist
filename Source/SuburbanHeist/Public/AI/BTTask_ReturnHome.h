#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReturnHome.generated.h"

/**
 * RETURN state (Section 9): walks back to HomeLocation, then resets the Resident to Idle
 * (clearing suspicion/target) via ASHResidentCharacter::Server_SetState.
 */
UCLASS()
class SUBURBANHEIST_API UBTTask_ReturnHome : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ReturnHome();

	UPROPERTY(EditAnywhere, Category = "Resident")
	float AcceptableRadius = 80.f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
