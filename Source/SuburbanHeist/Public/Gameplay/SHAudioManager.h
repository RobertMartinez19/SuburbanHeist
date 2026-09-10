#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SHAudioManager.generated.h"

class USoundBase;
class UAudioComponent;

/**
 * BP_AudioManager (Section 21). Level-placed, non-replicated - each client's own instance
 * plays sounds locally, so nothing here needs to be an RPC. Shared "everyone must hear this"
 * moments (victory/defeat/police) are triggered per-client because every client independently
 * receives the GameState phase change via replication and calls these Play* functions itself
 * (see Docs/UI_SPEC.md for the WBP_HUD binding that does this). Assign SoundBase placeholders
 * on the BP_AudioManager instance if final audio assets aren't available yet (Section 1/21).
 */
UCLASS()
class SUBURBANHEIST_API ASHAudioManager : public AActor
{
	GENERATED_BODY()

public:
	ASHAudioManager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|SFX")
	TObjectPtr<USoundBase> FootstepSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|SFX")
	TObjectPtr<USoundBase> DoorSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|SFX")
	TObjectPtr<USoundBase> InteractionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|SFX")
	TObjectPtr<USoundBase> ResidentDialogueSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|Stingers")
	TObjectPtr<USoundBase> DetectionStinger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|Music")
	TObjectPtr<USoundBase> ChaseMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|Stingers")
	TObjectPtr<USoundBase> VictoryStinger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|Stingers")
	TObjectPtr<USoundBase> DefeatStinger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio|Stingers")
	TObjectPtr<USoundBase> PoliceGameOverStinger;

	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayFootstep(FVector Location);
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayDoorSound(FVector Location);
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayInteractionSound(FVector Location);
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayResidentDialogue(FVector Location);
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayDetectionStinger();
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayChaseMusic();
	UFUNCTION(BlueprintCallable, Category = "Audio") void StopChaseMusic();
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayVictoryStinger();
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayDefeatStinger();
	UFUNCTION(BlueprintCallable, Category = "Audio") void PlayPoliceGameOverStinger();

private:
	UPROPERTY()
	TObjectPtr<UAudioComponent> ChaseMusicComponent;
};
