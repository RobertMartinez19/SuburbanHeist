#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHTypes.h"
#include "SHMicrophoneNoiseComponent.generated.h"

class UAudioCaptureComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnNoiseValueChanged, float, NoiseValue0to100);

/**
 * Section 6: optional microphone loudness -> gameplay noise. Amplitude only, never speech
 * recognition. Wraps the AudioCapture plugin's UAudioCaptureComponent envelope-follower so we
 * never touch raw PCM. When USHGameInstance::bMicrophoneDetectionEnabled is false, this
 * component stays dormant and ASHPlayerCharacter's action-based noise (Section 6/7) is the
 * sole source of player-caused noise instead.
 */
UCLASS(ClassGroup = (SuburbanHeist), meta = (BlueprintSpawnableComponent))
class SUBURBANHEIST_API USHMicrophoneNoiseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USHMicrophoneNoiseComponent();

	/** How often (seconds) we sample the capture envelope and possibly report noise (Section 6). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Microphone")
	float SampleInterval = 0.25f;

	/** Linear envelope [0,1] below this is treated as silence and not reported. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Microphone", meta = (ClampMin = "0", ClampMax = "1"))
	float SilenceFloor = 0.03f;

	/** Scales linear envelope -> 0-100 gameplay noise value. Tune per-mic-hardware in-game via settings. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Microphone")
	float SensitivityMultiplier = 220.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Microphone")
	float NoiseReportRadius = 1200.f;

	UPROPERTY(BlueprintAssignable, Category = "Microphone")
	FSHOnNoiseValueChanged OnNoiseValueChanged;

	UFUNCTION(BlueprintPure, Category = "Microphone")
	float GetLastNoiseValue() const { return LastNoiseValue; }

	UFUNCTION(BlueprintCallable, Category = "Microphone")
	void SetMicrophoneEnabled(bool bEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleAudioEnvelopeValue(const float EnvelopeValue);

	UFUNCTION(Server, Unreliable)
	void Server_ReportVoiceNoise(float NoiseValue0to100);

private:
	UPROPERTY()
	TObjectPtr<UAudioCaptureComponent> AudioCapture = nullptr;

	float LastNoiseValue = 0.f;
	bool bMicActive = false;
};
