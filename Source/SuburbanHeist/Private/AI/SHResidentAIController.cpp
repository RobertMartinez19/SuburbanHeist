#include "AI/SHResidentAIController.h"
#include "AI/SHResidentCharacter.h"
#include "AI/SHAIBlackboardKeys.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BrainComponent.h"

ASHResidentAIController::ASHResidentAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 200.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->SetMaxAge(5.f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ASHResidentAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASHResidentCharacter* Resident = Cast<ASHResidentCharacter>(InPawn);
	if (!Resident)
	{
		return;
	}

	// Archetype-specific perception tuning (Section 8/26) - applied per-instance, not per-class.
	const FSHResidentArchetypeDataRow& Tuning = Resident->GetTuning();
	SightConfig->SightRadius = Tuning.SightRadius;
	SightConfig->LoseSightRadius = Tuning.SightRadius + 200.f;
	SightConfig->PeripheralVisionAngleDegrees = Tuning.SightHalfAngleDegrees;
	HearingConfig->HearingRange = Tuning.HearingRadius;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ASHResidentAIController::HandleTargetPerceptionUpdated);

	if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
	{
		UBlackboardComponent* BB = Blackboard;
		UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BB);
		Blackboard = BB;
		Blackboard->SetValueAsVector(SHBlackboardKeys::HomeLocation, Resident->HomeLocation);
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ASHResidentAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ASHResidentCharacter* Resident = Cast<ASHResidentCharacter>(GetPawn());
	if (!Resident || !Actor || !Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	const FSHResidentArchetypeDataRow& Tuning = Resident->GetTuning();

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		// Direct line-of-sight is the strongest suspicion source - archetypes with a lower
		// DetectionThreshold will confirm a detection within a couple of sight ticks.
		Resident->Server_AddSuspicion(35.f, Actor, Stimulus.StimulusLocation);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		const float LoudnessPercent = Stimulus.Strength * 100.f;
		if (LoudnessPercent < Tuning.HearingNoiseThreshold)
		{
			return;
		}
		Resident->Server_AddSuspicion(LoudnessPercent * 0.5f, Actor, Stimulus.StimulusLocation);
	}
}

void ASHResidentAIController::Debug_SetAIPaused(bool bPaused)
{
	bAIPaused = bPaused;
	if (UBrainComponent* Brain = GetBrainComponent())
	{
		if (bPaused)
		{
			Brain->StopLogic(TEXT("Debug_ToggleAI"));
		}
		else if (BehaviorTreeAsset)
		{
			Brain->RestartLogic();
		}
	}
}
