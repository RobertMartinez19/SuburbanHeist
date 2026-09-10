#include "SHGameState.h"
#include "Net/UnrealNetwork.h"

ASHGameState::ASHGameState()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ASHGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASHGameState, GamePhase);
	DOREPLIFETIME(ASHGameState, MatchTimeRemaining);
	DOREPLIFETIME(ASHGameState, TeamMoney);
	DOREPLIFETIME(ASHGameState, UniqueResidentsDetected);
}

void ASHGameState::OnRep_GamePhase()
{
	OnPhaseChanged.Broadcast(GamePhase);
}

void ASHGameState::OnRep_MatchTimeRemaining()
{
	OnTimeChanged.Broadcast(MatchTimeRemaining);
}

void ASHGameState::OnRep_TeamMoney()
{
	OnMoneyChanged.Broadcast(TeamMoney);
}

void ASHGameState::OnRep_UniqueResidentsDetected()
{
	OnDetectionChanged.Broadcast(UniqueResidentsDetected);
}
