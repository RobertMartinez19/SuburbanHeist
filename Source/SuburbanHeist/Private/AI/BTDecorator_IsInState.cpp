#include "AI/BTDecorator_IsInState.h"
#include "AI/SHResidentCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTDecorator_IsInState::UBTDecorator_IsInState()
{
	NodeName = TEXT("Is In State");
}

bool UBTDecorator_IsInState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	return Resident && Resident->CurrentState == RequiredState;
}

FString UBTDecorator_IsInState::GetStaticDescription() const
{
	return FString::Printf(TEXT("Is In State: %s"), *UEnum::GetValueAsString(RequiredState));
}
