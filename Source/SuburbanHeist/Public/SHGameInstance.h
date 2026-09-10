#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SHGameInstance.generated.h"

/**
 * Session-wide settings that must survive travel between Lobby and the match map
 * (Section 6 mic on/off, Section 24 debug gate). Not replicated - each client and
 * the (listen/dedicated) server each keep their own instance.
 */
UCLASS()
class SUBURBANHEIST_API USHGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/** Master switch for ASHCheatManager exec commands (Section 24). Off by default in shipping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	bool bDebugModeEnabled =
#if UE_BUILD_SHIPPING
		false;
#else
		true;
#endif

	/** Section 6: "Microphone Detection: ON/OFF" - local player preference, read by
	 *  USHMicrophoneNoiseComponent. When off, action-based noise (Section 6/7) is used instead. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	bool bMicrophoneDetectionEnabled = true;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	bool IsDebugModeEnabled() const { return bDebugModeEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool IsMicrophoneDetectionEnabled() const { return bMicrophoneDetectionEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMicrophoneDetectionEnabled(bool bEnabled) { bMicrophoneDetectionEnabled = bEnabled; }
};
