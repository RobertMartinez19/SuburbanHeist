#include "SHPlayerController.h"
#include "SHCheatManager.h"

ASHPlayerController::ASHPlayerController()
{
	CheatClass = USHCheatManager::StaticClass();
}

void ASHPlayerController::Client_NotifyResidentSuspicious_Implementation()
{
	OnResidentSuspiciousNotify();
}

void ASHPlayerController::Client_NotifyResidentDetected_Implementation()
{
	OnResidentDetectedNotify();
}
