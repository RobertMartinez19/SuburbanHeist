#include "Player/SHMicrophoneNoiseComponent.h"
#include "AudioCaptureComponent.h"
#include "SHGameInstance.h"
#include "Gameplay/SHNoiseManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

// NOTE: UAudioCaptureComponent / envelope-follower delegate signatures have shifted slightly
// across 5.x point releases. If HandleAudioEnvelopeValue's binding fails to compile against
// your installed engine, adjust the signature to match AudioCaptureComponent.h /
// SynthComponent.h for that version - see Docs/LIMITATIONS.md.

USHMicrophoneNoiseComponent::USHMicrophoneNoiseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);

	AudioCapture = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCapture"));
	AudioCapture->bAutoActivate = false;
}

void USHMicrophoneNoiseComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
	{
		// Only the owning client captures its own microphone - never simulate mic input
		// for remote proxies, and never run capture on a dedicated server.
		return;
	}

	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const USHGameInstance* SHGI = Cast<USHGameInstance>(GI);
	SetMicrophoneEnabled(SHGI ? SHGI->IsMicrophoneDetectionEnabled() : true);
}

void USHMicrophoneNoiseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AudioCapture && bMicActive)
	{
		AudioCapture->Stop();
	}
	Super::EndPlay(EndPlayReason);
}

void USHMicrophoneNoiseComponent::SetMicrophoneEnabled(bool bEnabled)
{
	if (!AudioCapture || bEnabled == bMicActive)
	{
		return;
	}

	bMicActive = bEnabled;

	if (bEnabled)
	{
		AudioCapture->SetEnableEnvelopeFollowing(true);
		AudioCapture->OnAudioEnvelopeValue.AddDynamic(this, &USHMicrophoneNoiseComponent::HandleAudioEnvelopeValue);
		AudioCapture->Start();
	}
	else
	{
		AudioCapture->OnAudioEnvelopeValue.RemoveDynamic(this, &USHMicrophoneNoiseComponent::HandleAudioEnvelopeValue);
		AudioCapture->Stop();
		LastNoiseValue = 0.f;
		OnNoiseValueChanged.Broadcast(0.f);
	}
}

void USHMicrophoneNoiseComponent::HandleAudioEnvelopeValue(const float EnvelopeValue)
{
	if (EnvelopeValue < SilenceFloor)
	{
		if (LastNoiseValue != 0.f)
		{
			LastNoiseValue = 0.f;
			OnNoiseValueChanged.Broadcast(0.f);
		}
		return;
	}

	const float NoiseValue = FMath::Clamp(EnvelopeValue * SensitivityMultiplier, 0.f, 100.f);
	LastNoiseValue = NoiseValue;
	OnNoiseValueChanged.Broadcast(NoiseValue);

	// Voice noise maps to +10..+40 world noise per Section 7; scale the 0-100 loudness band down.
	const float WorldNoiseContribution = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 100.f), FVector2D(10.f, 40.f), NoiseValue);
	Server_ReportVoiceNoise(WorldNoiseContribution);
}

void USHMicrophoneNoiseComponent::Server_ReportVoiceNoise_Implementation(float NoiseValue0to100)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	TArray<AActor*> Managers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), Managers);
	if (Managers.Num() > 0)
	{
		if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(Managers[0]))
		{
			FSHNoiseEvent Event;
			Event.Location = OwnerActor->GetActorLocation();
			Event.Loudness = NoiseValue0to100;
			Event.Radius = NoiseReportRadius;
			Event.SourceType = ENoiseSourceType::Voice;
			Event.Instigator = OwnerActor;
			Noise->Server_ReportNoise(Event);
		}
	}
}
