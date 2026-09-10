#include "AI/BTTask_ResidentAttack.h"
#include "AI/SHResidentCharacter.h"
#include "AIController.h"

UBTTask_ResidentAttack::UBTTask_ResidentAttack()
{
	NodeName = TEXT("Resident Attack (Fictional)");
	bNotifyTick = true;
}

uint16 UBTTask_ResidentAttack::GetInstanceMemorySize() const
{
	return sizeof(FMemory);
}

EBTNodeResult::Type UBTTask_ResidentAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	AActor* Target = Resident ? Resident->TargetActor.Get() : nullptr;

	if (!Resident || !Target || !Resident->CanAttack())
	{
		return EBTNodeResult::Failed;
	}

	const float TimeSinceLastAttack = Resident->GetWorld()->GetTimeSeconds() - Resident->LastAttackWorldTime;
	if (TimeSinceLastAttack < Cooldown)
	{
		return EBTNodeResult::Failed;
	}

	if (FVector::Dist(Resident->GetActorLocation(), Target->GetActorLocation()) > Range)
	{
		return EBTNodeResult::Failed;
	}

	// Telegraph fires immediately so the player has the full TelegraphSeconds window to escape.
	Resident->Server_BeginAttackTelegraph(Target);

	FMemory* Memory = reinterpret_cast<FMemory*>(NodeMemory);
	Memory->ElapsedTelegraph = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_ResidentAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FMemory* Memory = reinterpret_cast<FMemory*>(NodeMemory);
	Memory->ElapsedTelegraph += DeltaSeconds;
	if (Memory->ElapsedTelegraph < TelegraphSeconds)
	{
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	ASHResidentCharacter* Resident = AIController ? Cast<ASHResidentCharacter>(AIController->GetPawn()) : nullptr;
	AActor* Target = Resident ? Resident->TargetActor.Get() : nullptr;

	// Players can escape the telegraph window (Section 11) by breaking range before it resolves.
	if (Resident && Target)
	{
		Resident->Server_MarkAttackAttempted();
		if (FVector::Dist(Resident->GetActorLocation(), Target->GetActorLocation()) <= Range && FMath::FRand() <= Accuracy)
		{
			Resident->Server_PerformChaseAttack(Target);
		}
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
