#include "AI/BTTask_ReturnHome.h"
#include "AI/SHResidentCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_ReturnHome::UBTTask_ReturnHome()
{
	NodeName = TEXT("Return Home");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ReturnHome::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	if (!AIController || !Resident)
	{
		return EBTNodeResult::Failed;
	}

	const FPathFollowingRequestResult MoveResult = AIController->MoveToLocation(Resident->HomeLocation, AcceptableRadius);
	switch (MoveResult.Code)
	{
		case EPathFollowingRequestResult::AlreadyAtGoal:
			Resident->Server_SetState(EResidentState::Idle);
			return EBTNodeResult::Succeeded;
		case EPathFollowingRequestResult::RequestSuccessful:
			return EBTNodeResult::InProgress;
		default:
			return EBTNodeResult::Failed;
	}
}

void UBTTask_ReturnHome::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	UPathFollowingComponent* PathFollowing = AIController ? AIController->GetPathFollowingComponent() : nullptr;
	if (!PathFollowing || !Resident)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (PathFollowing->GetStatus() == EPathFollowingStatus::Idle)
	{
		Resident->Server_SetState(EResidentState::Idle);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
