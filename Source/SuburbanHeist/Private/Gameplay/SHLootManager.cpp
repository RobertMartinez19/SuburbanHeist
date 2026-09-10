#include "Gameplay/SHLootManager.h"
#include "SHGameState.h"
#include "SHGameMode.h"
#include "SHPlayerState.h"

ASHLootManager::ASHLootManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false; // TeamMoney lives on GameState; this actor only runs server-side logic.
}

void ASHLootManager::Server_AwardMoney(float Amount, APlayerState* ContributingPlayer)
{
	if (!HasAuthority() || Amount <= 0.f)
	{
		return;
	}

	ASHGameState* GS = GetWorld()->GetGameState<ASHGameState>();
	if (!GS || GS->GamePhase != EGamePhase::Active)
	{
		return;
	}

	GS->TeamMoney += Amount;
	GS->OnRep_TeamMoney();
	OnMoneyChanged.Broadcast(GS->TeamMoney);

	if (ASHPlayerState* SHPS = Cast<ASHPlayerState>(ContributingPlayer))
	{
		SHPS->AddContribution(Amount);
	}

	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Server_CheckWinCondition();
	}

	if (GS->TeamMoney >= GS->GoalMoney)
	{
		OnTeamReachedGoal.Broadcast();
	}
}

void ASHLootManager::Server_NotifyObjectiveCompleted(AActor* ObjectivePoint, float Value, APlayerState* ContributingPlayer)
{
	if (!HasAuthority())
	{
		return;
	}
	OnObjectiveCompleted.Broadcast(ObjectivePoint, Value);
	Server_AwardMoney(Value, ContributingPlayer);
}
