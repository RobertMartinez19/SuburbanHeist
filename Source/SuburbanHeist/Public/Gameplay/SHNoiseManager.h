#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SHTypes.h"
#include "SHNoiseManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnNoiseGenerated, const FSHNoiseEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnTeamNoiseLevelChanged, float, TeamNoiseLevel0to100);

/**
 * BP_NoiseManager (Section 7). Server-authoritative: converts every noise-causing gameplay
 * event into an AI hearing stimulus (radius-based propagation) and maintains a decaying
 * 0-100 "TeamNoiseLevel" purely for the UI meter (Section 20). One instance per map.
 */
UCLASS()
class SUBURBANHEIST_API ASHNoiseManager : public AActor
{
	GENERATED_BODY()

public:
	ASHNoiseManager();

	/** Debug toggle (Section 24 ToggleNoise). While false, noise events are ignored entirely. */
	UPROPERTY(BlueprintReadWrite, Category = "Noise|Debug")
	bool bNoiseSystemEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	float DecayPerSecond = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	float DecayTickInterval = 0.25f;

	UPROPERTY(ReplicatedUsing = OnRep_TeamNoiseLevel, BlueprintReadOnly, Category = "Noise")
	float TeamNoiseLevel = 0.f;

	UPROPERTY(BlueprintAssignable, Category = "Noise|Events")
	FSHOnNoiseGenerated OnNoiseGenerated;

	UPROPERTY(BlueprintAssignable, Category = "Noise|Events")
	FSHOnTeamNoiseLevelChanged OnTeamNoiseLevelChanged;

	/** Server entry point for every noise source in the game (Section 7). */
	UFUNCTION(BlueprintCallable, Category = "Noise")
	void Server_ReportNoise(const FSHNoiseEvent& Event);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_TeamNoiseLevel();
	void Server_TickDecay();

private:
	FTimerHandle DecayTimerHandle;
};
