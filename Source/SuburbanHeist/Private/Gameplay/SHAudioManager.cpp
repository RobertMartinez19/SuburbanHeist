#include "Gameplay/SHAudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

ASHAudioManager::ASHAudioManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void ASHAudioManager::PlayFootstep(FVector Location)
{
	if (FootstepSound) UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, Location);
}

void ASHAudioManager::PlayDoorSound(FVector Location)
{
	if (DoorSound) UGameplayStatics::PlaySoundAtLocation(this, DoorSound, Location);
}

void ASHAudioManager::PlayInteractionSound(FVector Location)
{
	if (InteractionSound) UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, Location);
}

void ASHAudioManager::PlayResidentDialogue(FVector Location)
{
	if (ResidentDialogueSound) UGameplayStatics::PlaySoundAtLocation(this, ResidentDialogueSound, Location);
}

void ASHAudioManager::PlayDetectionStinger()
{
	if (DetectionStinger) UGameplayStatics::PlaySound2D(this, DetectionStinger);
}

void ASHAudioManager::PlayChaseMusic()
{
	if (ChaseMusic && !ChaseMusicComponent)
	{
		ChaseMusicComponent = UGameplayStatics::SpawnSound2D(this, ChaseMusic);
	}
}

void ASHAudioManager::StopChaseMusic()
{
	if (ChaseMusicComponent)
	{
		ChaseMusicComponent->Stop();
		ChaseMusicComponent = nullptr;
	}
}

void ASHAudioManager::PlayVictoryStinger()
{
	if (VictoryStinger) UGameplayStatics::PlaySound2D(this, VictoryStinger);
}

void ASHAudioManager::PlayDefeatStinger()
{
	if (DefeatStinger) UGameplayStatics::PlaySound2D(this, DefeatStinger);
}

void ASHAudioManager::PlayPoliceGameOverStinger()
{
	if (PoliceGameOverStinger) UGameplayStatics::PlaySound2D(this, PoliceGameOverStinger);
}
