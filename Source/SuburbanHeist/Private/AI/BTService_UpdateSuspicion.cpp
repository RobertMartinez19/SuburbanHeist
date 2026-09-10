#include "AI/BTService_UpdateSuspicion.h"
#include "AI/SHResidentCharacter.h"
#include "AI/SHAIBlackboardKeys.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateSuspicion::UBTService_UpdateSuspicion()
{
	NodeName = TEXT("Update Suspicion");
	Interval = 0.5f;
	RandomDeviation = 0.05f;
}

void UBTService_UpdateSuspicion::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!Resident || !BB)
	{
		return;
	}

	Resident->Server_TickSuspicionDecay(DeltaSeconds);

	BB->SetValueAsFloat(SHBlackboardKeys::SuspicionLevel, Resident->SuspicionLevel);
	BB->SetValueAsObject(SHBlackboardKeys::TargetActor, Resident->TargetActor.Get());
	BB->SetValueAsVector(SHBlackboardKeys::LastHeardLocation, Resident->LastHeardLocation);
	BB->SetValueAsBool(SHBlackboardKeys::IsInvestigating, Resident->CurrentState == EResidentState::Investigating || Resident->CurrentState == EResidentState::Alert);
	BB->SetValueAsBool(SHBlackboardKeys::HasDetectedPlayer, Resident->CurrentState == EResidentState::Detected || Resident->CurrentState == EResidentState::Chase);
	BB->SetValueAsEnum(SHBlackboardKeys::DetectionState, static_cast<uint8>(Resident->CurrentState));
}
