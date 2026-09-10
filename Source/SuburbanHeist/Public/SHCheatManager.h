#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SHCheatManager.generated.h"

/**
 * Debug console commands (Section 24). Every exec function first checks
 * USHGameInstance::bDebugModeEnabled and bails if debug mode is off. Only server-authoritative
 * exec commands actually mutate state; on a client they are routed to the server via the
 * owning PlayerController (standard UCheatManager behavior - cheat managers only exist on
 * locally controlled, and if `AllowCheats`-flagged, connections).
 */
UCLASS()
class SUBURBANHEIST_API USHCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void GiveMoney(float Amount = 500.f);

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void TriggerDetection();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void ResetDetection();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void SpawnResident(FString ArchetypeName = TEXT("Calm"));

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void CompleteObjective();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void SetTimer(float Seconds = 60.f);

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void WinMatch();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void LoseMatch();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void ToggleNoise();

	UFUNCTION(Exec, Category = "SuburbanHeist|Debug")
	void ToggleAI();

private:
	bool IsDebugAllowed() const;
};
