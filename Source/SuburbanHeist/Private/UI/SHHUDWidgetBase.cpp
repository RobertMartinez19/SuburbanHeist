#include "UI/SHHUDWidgetBase.h"
#include "SHGameState.h"
#include "Gameplay/SHNoiseManager.h"
#include "Kismet/GameplayStatics.h"

void USHHUDWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASHGameState* GS = GetWorld()->GetGameState<ASHGameState>())
	{
		GS->OnTimeChanged.AddDynamic(this, &USHHUDWidgetBase::HandleTimeChanged);
		GS->OnMoneyChanged.AddDynamic(this, &USHHUDWidgetBase::HandleMoneyChanged);
		GS->OnDetectionChanged.AddDynamic(this, &USHHUDWidgetBase::HandleDetectionChanged);
		GS->OnPhaseChanged.AddDynamic(this, &USHHUDWidgetBase::HandlePhaseChanged);

		// Prime the UI with current values immediately, don't wait for the next change.
		HandleTimeChanged(GS->MatchTimeRemaining);
		HandleMoneyChanged(GS->TeamMoney);
		HandleDetectionChanged(GS->UniqueResidentsDetected);
		HandlePhaseChanged(GS->GamePhase);
	}

	TArray<AActor*> NoiseManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), NoiseManagers);
	if (NoiseManagers.Num() > 0)
	{
		if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(NoiseManagers[0]))
		{
			Noise->OnTeamNoiseLevelChanged.AddDynamic(this, &USHHUDWidgetBase::HandleNoiseChanged);
		}
	}
}

void USHHUDWidgetBase::NativeDestruct()
{
	if (ASHGameState* GS = GetWorld() ? GetWorld()->GetGameState<ASHGameState>() : nullptr)
	{
		GS->OnTimeChanged.RemoveDynamic(this, &USHHUDWidgetBase::HandleTimeChanged);
		GS->OnMoneyChanged.RemoveDynamic(this, &USHHUDWidgetBase::HandleMoneyChanged);
		GS->OnDetectionChanged.RemoveDynamic(this, &USHHUDWidgetBase::HandleDetectionChanged);
		GS->OnPhaseChanged.RemoveDynamic(this, &USHHUDWidgetBase::HandlePhaseChanged);
	}
	Super::NativeDestruct();
}

FText USHHUDWidgetBase::FormatTime(float Seconds) const
{
	const int32 TotalSeconds = FMath::Max(0, FMath::CeilToInt(Seconds));
	const int32 Minutes = TotalSeconds / 60;
	const int32 Secs = TotalSeconds % 60;
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Secs));
}

void USHHUDWidgetBase::HandleTimeChanged(float NewTimeRemaining)
{
	OnTimeRemainingChanged(NewTimeRemaining);
}

void USHHUDWidgetBase::HandleMoneyChanged(float NewTeamMoney)
{
	if (const ASHGameState* GS = GetWorld()->GetGameState<ASHGameState>())
	{
		OnTeamCashChanged(NewTeamMoney, GS->GoalMoney);
	}
}

void USHHUDWidgetBase::HandleDetectionChanged(int32 NewUniqueResidentsDetected)
{
	if (const ASHGameState* GS = GetWorld()->GetGameState<ASHGameState>())
	{
		OnDetectionChanged(NewUniqueResidentsDetected, GS->DetectionLimit);
	}
}

void USHHUDWidgetBase::HandleNoiseChanged(float NewNoiseLevel)
{
	OnNoiseLevelChanged(NewNoiseLevel);
}

void USHHUDWidgetBase::HandlePhaseChanged(EGamePhase NewPhase)
{
	OnGamePhaseChanged(NewPhase);
}
