#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SHTypes.h"
#include "SHGameMode.generated.h"

class ASHGameState;
class ASHTeamDetectionManager;
class ASHLootManager;
class ASHNoiseManager;
class ASHAudioManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnVictory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnDefeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnPoliceCalled);

/**
 * Server-authoritative match flow controller (Sections 4, 19, 24, 30-Phase5/9).
 * Owns the 20-minute countdown and all phase transitions. Never trust client input for
 * anything here - GiveMoney/TriggerDetection/etc are debug-only and gated by
 * ASHGameInstance::bDebugModeEnabled, exercised through ASHCheatManager.
 */
UCLASS(Config = Game)
class SUBURBANHEIST_API ASHGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASHGameMode();

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	float MatchDurationSeconds = 1200.f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	float GoalMoney = 10000.f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	int32 DetectionLimit = 3;

	/** Countdown shown before the match timer starts (Section 4, step 2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	float PreMatchCountdownSeconds = 5.f;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnVictory OnVictory;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnDefeat OnDefeat;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnPoliceCalled OnPoliceCalled;

	virtual void StartPlay() override;

	/** Called by ASHTeamDetectionManager when UniqueResidentsDetected reaches DetectionLimit. */
	void Server_GameOver_Police();

	/** Called by ASHLootManager whenever TeamMoney changes; checks the win condition. */
	void Server_CheckWinCondition();

	/** Resets money/detection/objectives/residents and starts a fresh countdown (Section 31
	 *  acceptance test #18 "complete another match"). Wire to the "Play Again" button on
	 *  WBP_Victory/WBP_Defeat/WBP_PoliceGameOver (Docs/UI_SPEC.md). */
	UFUNCTION(BlueprintCallable, Category = "Match")
	void Server_RestartMatch();

	// -- Debug-only hooks (Section 24), exercised via ASHCheatManager --
	void Debug_GiveMoney(float Amount);
	void Debug_TriggerDetection();
	void Debug_ResetDetection();
	void Debug_SetTimer(float NewSeconds);
	void Debug_WinMatch();
	void Debug_LoseMatch();

	UFUNCTION(BlueprintPure, Category = "Match")
	ASHTeamDetectionManager* GetDetectionManager() const { return DetectionManager; }

	UFUNCTION(BlueprintPure, Category = "Match")
	ASHLootManager* GetLootManager() const { return LootManager; }

	UFUNCTION(BlueprintPure, Category = "Match")
	ASHNoiseManager* GetNoiseManager() const { return NoiseManager; }

	UFUNCTION(BlueprintPure, Category = "Match")
	ASHAudioManager* GetAudioManager() const { return AudioManager; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<ASHGameState> SHGameState = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASHTeamDetectionManager> DetectionManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASHLootManager> LootManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASHNoiseManager> NoiseManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASHAudioManager> AudioManager = nullptr;

	FTimerHandle CountdownTimerHandle;
	FTimerHandle MatchTickTimerHandle;

	void FindOrSpawnManagers();
	void SetPhase(EGamePhase NewPhase);
	void BeginCountdown();
	void BeginActiveMatch();

	/** Runs once per second on the server; coarse-grained to keep replication cheap (Section 27). */
	void Server_TickMatchTimer();

	void EnterVictory();
	void EnterDefeat();
	void EnterPoliceGameOver();
};
