#include "Gameplay/SHTeamDetectionManager.h"
#include "SHGameState.h"
#include "SHGameMode.h"
#include "SHPlayerState.h"
#include "GameFramework/Character.h"

ASHTeamDetectionManager::ASHTeamDetectionManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // exists identically on server+clients; only server ever mutates it
}

void ASHTeamDetectionManager::Server_ReportDetection(AActor* ResidentActor, AActor* DetectedPlayer)
{
	if (!HasAuthority() || !ResidentActor)
	{
		return;
	}

	if (DetectedPlayer)
	{
		if (ASHPlayerState* PS = Cast<ASHPlayerState>(Cast<ACharacter>(DetectedPlayer) ? Cast<ACharacter>(DetectedPlayer)->GetPlayerState() : nullptr))
		{
			PS->bIsCurrentlyDetected = true;
		}
	}

	// Same resident detecting a second player must NOT increment the count again (Section 5).
	if (DetectedResidents.ContainsByPredicate([ResidentActor](const TWeakObjectPtr<AActor>& Ptr) { return Ptr.Get() == ResidentActor; }))
	{
		return;
	}

	DetectedResidents.Add(ResidentActor);
	PushCountToGameState();
	OnUniqueResidentDetected.Broadcast(ResidentActor, GetUniqueResidentsDetectedCount());

	if (GetUniqueResidentsDetectedCount() >= DetectionLimit)
	{
		OnThirdResidentDetected.Broadcast();
		if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
		{
			GM->Server_GameOver_Police();
		}
	}
}

void ASHTeamDetectionManager::Server_ResetAll()
{
	if (!HasAuthority())
	{
		return;
	}
	DetectedResidents.Reset();
	DebugForcedDetections = 0;
	PushCountToGameState();
}

void ASHTeamDetectionManager::Debug_ForceDetection()
{
	if (!HasAuthority())
	{
		return;
	}
	// Debug-only: bump a separate counter rather than fabricating a fake resident actor - a
	// NewObject<AActor> held only by a TWeakObjectPtr has nothing keeping it alive and could
	// be garbage-collected between calls, silently corrupting the real dedup set below.
	++DebugForcedDetections;
	PushCountToGameState();
	OnUniqueResidentDetected.Broadcast(nullptr, GetUniqueResidentsDetectedCount());

	if (GetUniqueResidentsDetectedCount() >= DetectionLimit)
	{
		OnThirdResidentDetected.Broadcast();
		if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
		{
			GM->Server_GameOver_Police();
		}
	}
}

void ASHTeamDetectionManager::PushCountToGameState()
{
	if (ASHGameState* GS = GetWorld()->GetGameState<ASHGameState>())
	{
		GS->UniqueResidentsDetected = GetUniqueResidentsDetectedCount();
		GS->OnRep_UniqueResidentsDetected();
	}
}
