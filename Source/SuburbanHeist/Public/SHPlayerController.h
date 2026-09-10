#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SHPlayerController.generated.h"

/**
 * Sets the debug CheatManager class (Section 24) and forwards a couple of purely
 * cosmetic client notifications that don't warrant their own replicated state.
 */
UCLASS()
class SUBURBANHEIST_API ASHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASHPlayerController();

	/** Called by ASHTeamDetectionManager (server) to flash a HUD warning the instant ANY
	 *  resident becomes suspicious of ANY player, even before a detection is confirmed. */
	UFUNCTION(Client, Reliable, Category = "UI")
	void Client_NotifyResidentSuspicious();

	UFUNCTION(Client, Reliable, Category = "UI")
	void Client_NotifyResidentDetected();

	/** WBP_Victory/WBP_Defeat/WBP_PoliceGameOver's "Play Again" button calls this (Docs/UI_SPEC.md)
	 *  rather than reaching for ASHGameMode directly - AGameMode only exists on the server, so a
	 *  client Blueprint has no valid GameMode reference to call into; PlayerController is the
	 *  one framework class guaranteed to exist and route RPCs on both ends. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Match")
	void Server_RequestRestartMatch();

protected:
	/** Bind in WBP_HUD (Docs/UI_SPEC.md) to flash a "someone noticed something" cue. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnResidentSuspiciousNotify();

	/** Bind in WBP_HUD to flash the stronger "you've been spotted" cue. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnResidentDetectedNotify();
};
