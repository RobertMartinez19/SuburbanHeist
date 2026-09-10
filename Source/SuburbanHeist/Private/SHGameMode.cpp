#include "SHGameMode.h"
#include "SHGameState.h"
#include "SuburbanHeist.h"
#include "Gameplay/SHTeamDetectionManager.h"
#include "Gameplay/SHLootManager.h"
#include "Gameplay/SHNoiseManager.h"
#include "Gameplay/SHAudioManager.h"
#include "UI/SHHUD.h"
#include "SHPlayerState.h"
#include "SHPlayerController.h"
#include "Player/SHPlayerCharacter.h"
#include "Gameplay/SHObjectivePoint.h"
#include "AI/SHResidentCharacter.h"
#include "Kismet/GameplayStatics.h"

ASHGameMode::ASHGameMode()
{
	GameStateClass = ASHGameState::StaticClass();
	PlayerStateClass = ASHPlayerState::StaticClass();
	PlayerControllerClass = ASHPlayerController::StaticClass();
	DefaultPawnClass = ASHPlayerCharacter::StaticClass();
	HUDClass = ASHHUD::StaticClass();
	PrimaryActorTick.bCanEverTick = false;
}

void ASHGameMode::StartPlay()
{
	Super::StartPlay();

	SHGameState = GetGameState<ASHGameState>();
	if (SHGameState)
	{
		SHGameState->GoalMoney = GoalMoney;
		SHGameState->DetectionLimit = DetectionLimit;
		SHGameState->MatchTimeRemaining = MatchDurationSeconds;
	}

	FindOrSpawnManagers();
	SetPhase(EGamePhase::Lobby);
	BeginCountdown();
}

void ASHGameMode::FindOrSpawnManagers()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// These managers are expected to be placed in the map (BP_TeamDetectionManager,
	// BP_LootManager, BP_NoiseManager, BP_AudioManager). If a designer forgot to place one,
	// spawn the C++ base as a safe fallback so the match can still function.
	TArray<AActor*> Found;

	UGameplayStatics::GetAllActorsOfClass(World, ASHTeamDetectionManager::StaticClass(), Found);
	DetectionManager = Found.Num() > 0 ? Cast<ASHTeamDetectionManager>(Found[0]) : World->SpawnActor<ASHTeamDetectionManager>();

	Found.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, ASHLootManager::StaticClass(), Found);
	LootManager = Found.Num() > 0 ? Cast<ASHLootManager>(Found[0]) : World->SpawnActor<ASHLootManager>();

	Found.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, ASHNoiseManager::StaticClass(), Found);
	NoiseManager = Found.Num() > 0 ? Cast<ASHNoiseManager>(Found[0]) : World->SpawnActor<ASHNoiseManager>();

	Found.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, ASHAudioManager::StaticClass(), Found);
	AudioManager = Found.Num() > 0 ? Cast<ASHAudioManager>(Found[0]) : World->SpawnActor<ASHAudioManager>();

	if (DetectionManager)
	{
		DetectionManager->DetectionLimit = DetectionLimit;
	}
}

void ASHGameMode::SetPhase(EGamePhase NewPhase)
{
	if (SHGameState)
	{
		SHGameState->GamePhase = NewPhase;
		SHGameState->OnRep_GamePhase(); // server has no RepNotify call, broadcast locally too
	}
}

void ASHGameMode::BeginCountdown()
{
	SetPhase(EGamePhase::Countdown);
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ASHGameMode::BeginActiveMatch, PreMatchCountdownSeconds, false);
}

void ASHGameMode::BeginActiveMatch()
{
	SetPhase(EGamePhase::Active);
	GetWorldTimerManager().SetTimer(MatchTickTimerHandle, this, &ASHGameMode::Server_TickMatchTimer, 1.f, true);
}

void ASHGameMode::Server_TickMatchTimer()
{
	if (!SHGameState || SHGameState->GamePhase != EGamePhase::Active)
	{
		return;
	}

	SHGameState->MatchTimeRemaining = FMath::Max(0.f, SHGameState->MatchTimeRemaining - 1.f);
	SHGameState->OnRep_MatchTimeRemaining();

	if (SHGameState->MatchTimeRemaining <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(MatchTickTimerHandle);
		if (SHGameState->TeamMoney >= SHGameState->GoalMoney)
		{
			EnterVictory();
		}
		else
		{
			EnterDefeat();
		}
	}
}

void ASHGameMode::Server_CheckWinCondition()
{
	if (!SHGameState || SHGameState->GamePhase != EGamePhase::Active)
	{
		return;
	}

	if (SHGameState->TeamMoney >= SHGameState->GoalMoney)
	{
		GetWorldTimerManager().ClearTimer(MatchTickTimerHandle);
		EnterVictory();
	}
}

void ASHGameMode::Server_GameOver_Police()
{
	if (!SHGameState || SHGameState->GamePhase != EGamePhase::Active)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(MatchTickTimerHandle);
	EnterPoliceGameOver();
}

void ASHGameMode::Server_RestartMatch()
{
	GetWorldTimerManager().ClearTimer(MatchTickTimerHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	if (SHGameState)
	{
		SHGameState->TeamMoney = 0.f;
		SHGameState->OnRep_TeamMoney();
		SHGameState->UniqueResidentsDetected = 0;
		SHGameState->OnRep_UniqueResidentsDetected();
		SHGameState->MatchTimeRemaining = MatchDurationSeconds;
		SHGameState->OnRep_MatchTimeRemaining();
	}

	if (DetectionManager)
	{
		DetectionManager->Server_ResetAll();
	}

	TArray<AActor*> Objectives;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHObjectivePoint::StaticClass(), Objectives);
	for (AActor* Actor : Objectives)
	{
		if (ASHObjectivePoint* Objective = Cast<ASHObjectivePoint>(Actor))
		{
			Objective->Server_ResetForRematch();
		}
	}

	TArray<AActor*> Residents;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHResidentCharacter::StaticClass(), Residents);
	for (AActor* Actor : Residents)
	{
		if (ASHResidentCharacter* Resident = Cast<ASHResidentCharacter>(Actor))
		{
			Resident->Server_ResetForRematch();
		}
	}

	BeginCountdown();
}

void ASHGameMode::EnterVictory()
{
	SetPhase(EGamePhase::Victory);
	OnVictory.Broadcast();
	if (AudioManager)
	{
		AudioManager->PlayVictoryStinger();
	}
}

void ASHGameMode::EnterDefeat()
{
	SetPhase(EGamePhase::Defeat);
	OnDefeat.Broadcast();
	if (AudioManager)
	{
		AudioManager->PlayDefeatStinger();
	}
}

void ASHGameMode::EnterPoliceGameOver()
{
	SetPhase(EGamePhase::PoliceGameOver);
	OnPoliceCalled.Broadcast();
	if (AudioManager)
	{
		AudioManager->PlayPoliceGameOverStinger();
	}
}

// ---------------------------------------------------------------------------
// Debug hooks (Section 24). Callers (ASHCheatManager) are responsible for
// gating these behind ASHGameInstance::bDebugModeEnabled.
// ---------------------------------------------------------------------------

void ASHGameMode::Debug_GiveMoney(float Amount)
{
	if (LootManager)
	{
		LootManager->Server_AwardMoney(Amount, nullptr);
	}
}

void ASHGameMode::Debug_TriggerDetection()
{
	if (DetectionManager)
	{
		DetectionManager->Debug_ForceDetection();
	}
}

void ASHGameMode::Debug_ResetDetection()
{
	if (DetectionManager)
	{
		DetectionManager->Server_ResetAll();
	}
}

void ASHGameMode::Debug_SetTimer(float NewSeconds)
{
	if (SHGameState)
	{
		SHGameState->MatchTimeRemaining = FMath::Max(0.f, NewSeconds);
		SHGameState->OnRep_MatchTimeRemaining();
	}
}

void ASHGameMode::Debug_WinMatch()
{
	EnterVictory();
}

void ASHGameMode::Debug_LoseMatch()
{
	EnterDefeat();
}
