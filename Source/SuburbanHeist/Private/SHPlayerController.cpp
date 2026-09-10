#include "SHPlayerController.h"
#include "SHCheatManager.h"
#include "SHGameMode.h"

ASHPlayerController::ASHPlayerController()
{
	CheatClass = USHCheatManager::StaticClass();
}

void ASHPlayerController::Server_RequestRestartMatch_Implementation()
{
	if (ASHGameMode* GM = GetWorld()->GetAuthGameMode<ASHGameMode>())
	{
		GM->Server_RestartMatch();
	}
}

void ASHPlayerController::Client_NotifyResidentSuspicious_Implementation()
{
	OnResidentSuspiciousNotify();
}

void ASHPlayerController::Client_NotifyResidentDetected_Implementation()
{
	OnResidentDetectedNotify();
}
