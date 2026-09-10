#include "SHPlayerState.h"
#include "Net/UnrealNetwork.h"

ASHPlayerState::ASHPlayerState()
{
}

void ASHPlayerState::AddContribution(float MoneyAmount)
{
	if (!HasAuthority())
	{
		return;
	}
	MoneyContributed += MoneyAmount;
	++ObjectivesCompleted;
}

void ASHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASHPlayerState, MoneyContributed);
	DOREPLIFETIME(ASHPlayerState, ObjectivesCompleted);
	DOREPLIFETIME(ASHPlayerState, bIsCurrentlyDetected);
}
