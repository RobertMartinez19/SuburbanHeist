#include "AI/BTTask_MoveToSuspicionLocation.h"
#include "AI/SHResidentCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveToSuspicionLocation::UBTTask_MoveToSuspicionLocation()
{
	NodeName = TEXT("Move To Suspicion Location");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToSuspicionLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	if (!AIController || !Resident)
	{
		return EBTNodeResult::Failed;
	}

	const EPathFollowingRequestResult::Type MoveResult = (bUseTargetActor && Resident->TargetActor.IsValid())
		? AIController->MoveToActor(Resident->TargetActor.Get(), AcceptableRadius)
		: AIController->MoveToLocation(Resident->LastHeardLocation, AcceptableRadius);

	switch (MoveResult)
	{
		case EPathFollowingRequestResult::AlreadyAtGoal:
			return EBTNodeResult::Succeeded;
		case EPathFollowingRequestResult::RequestSuccessful:
			return EBTNodeResult::InProgress;
		default:
			return EBTNodeResult::Failed;
	}
}

void UBTTask_MoveToSuspicionLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UPathFollowingComponent* PathFollowing = AIController ? AIController->GetPathFollowingComponent() : nullptr;
	if (!PathFollowing)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (PathFollowing->GetStatus() == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
