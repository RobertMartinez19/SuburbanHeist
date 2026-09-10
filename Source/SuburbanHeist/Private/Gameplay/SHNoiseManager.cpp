#include "Gameplay/SHNoiseManager.h"
#include "Perception/AISense_Hearing.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ASHNoiseManager::ASHNoiseManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(false);
}

void ASHNoiseManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASHNoiseManager, TeamNoiseLevel);
}

void ASHNoiseManager::Server_ReportNoise(const FSHNoiseEvent& Event)
{
	if (!HasAuthority() || !bNoiseSystemEnabled)
	{
		return;
	}

	// Feed AI perception directly - this is what actually makes Residents react (Section 10).
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(), Event.Location, FMath::Clamp(Event.Loudness / 100.f, 0.f, 1.f),
		Event.Instigator.Get(), Event.Radius);

	OnNoiseGenerated.Broadcast(Event);

	TeamNoiseLevel = FMath::Clamp(TeamNoiseLevel + Event.Loudness, 0.f, 100.f);
	OnRep_TeamNoiseLevel();

	if (!GetWorldTimerManager().IsTimerActive(DecayTimerHandle))
	{
		GetWorldTimerManager().SetTimer(DecayTimerHandle, this, &ASHNoiseManager::Server_TickDecay, DecayTickInterval, true);
	}
}

void ASHNoiseManager::Server_TickDecay()
{
	TeamNoiseLevel = FMath::Max(0.f, TeamNoiseLevel - DecayPerSecond * DecayTickInterval);
	OnRep_TeamNoiseLevel();

	if (TeamNoiseLevel <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(DecayTimerHandle);
	}
}

void ASHNoiseManager::OnRep_TeamNoiseLevel()
{
	OnTeamNoiseLevelChanged.Broadcast(TeamNoiseLevel);
}
