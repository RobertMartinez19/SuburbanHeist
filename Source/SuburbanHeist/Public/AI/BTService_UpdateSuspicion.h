#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSuspicion.generated.h"

/**
 * Periodic (not per-frame) service that decays suspicion / advances chase timeout on the
 * possessed ASHResidentCharacter and mirrors its state into the Blackboard so decorators and
 * tasks can branch on it (Section 9/27). Attach at the root of BT_Resident with Interval ~0.5s.
 */
UCLASS()
class SUBURBANHEIST_API UBTService_UpdateSuspicion : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateSuspicion();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
