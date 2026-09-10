#include "SHCheatManager.h"
#include "SHGameInstance.h"
#include "SHGameMode.h"
#include "Gameplay/SHLootManager.h"
#include "Gameplay/SHTeamDetectionManager.h"
#include "Gameplay/SHNoiseManager.h"
#include "Gameplay/SHObjectivePoint.h"
#include "AI/SHResidentCharacter.h"
#include "AI/SHResidentAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

bool USHCheatManager::IsDebugAllowed() const
{
	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const USHGameInstance* SHGI = Cast<USHGameInstance>(GI);
	return SHGI && SHGI->IsDebugModeEnabled();
}

void USHCheatManager::GiveMoney(float Amount)
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_GiveMoney(Amount);
	}
}

void USHCheatManager::TriggerDetection()
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_TriggerDetection();
	}
}

void USHCheatManager::ResetDetection()
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_ResetDetection();
	}
}

void USHCheatManager::SpawnResident(FString ArchetypeName)
{
	if (!IsDebugAllowed()) return;

	APlayerController* PC = GetOuterAPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn) return;

	const FVector SpawnLoc = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 300.f;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASHResidentCharacter* Resident = GetWorld()->SpawnActor<ASHResidentCharacter>(ASHResidentCharacter::StaticClass(), SpawnLoc, Pawn->GetActorRotation(), Params);
	if (Resident)
	{
		Resident->SetArchetypeByName(ArchetypeName);
	}
}

void USHCheatManager::CompleteObjective()
{
	if (!IsDebugAllowed()) return;

	TArray<AActor*> Objectives;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHObjectivePoint::StaticClass(), Objectives);
	for (AActor* Actor : Objectives)
	{
		if (ASHObjectivePoint* Objective = Cast<ASHObjectivePoint>(Actor))
		{
			if (!Objective->IsCompleted())
			{
				Objective->Debug_ForceComplete();
				break;
			}
		}
	}
}

void USHCheatManager::SetTimer(float Seconds)
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_SetTimer(Seconds);
	}
}

void USHCheatManager::WinMatch()
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_WinMatch();
	}
}

void USHCheatManager::LoseMatch()
{
	if (!IsDebugAllowed()) return;
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Debug_LoseMatch();
	}
}

void USHCheatManager::ToggleNoise()
{
	if (!IsDebugAllowed()) return;

	TArray<AActor*> Managers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), Managers);
	for (AActor* Actor : Managers)
	{
		if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(Actor))
		{
			Noise->bNoiseSystemEnabled = !Noise->bNoiseSystemEnabled;
		}
	}
}

void USHCheatManager::ToggleAI()
{
	if (!IsDebugAllowed()) return;

	TArray<AActor*> Residents;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHResidentCharacter::StaticClass(), Residents);
	for (AActor* Actor : Residents)
	{
		if (ASHResidentCharacter* Resident = Cast<ASHResidentCharacter>(Actor))
		{
			if (ASHResidentAIController* Controller = Cast<ASHResidentAIController>(Resident->GetController()))
			{
				Controller->Debug_SetAIPaused(!Controller->IsAIPaused());
			}
		}
	}
}
