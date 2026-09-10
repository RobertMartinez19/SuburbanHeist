#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHTypes.h"
#include "SHHUDWidgetBase.generated.h"

/**
 * C++ base for WBP_HUD (Section 20). Binds itself to ASHGameState/managers once in
 * NativeConstruct and forwards every change as a BlueprintImplementableEvent so designers
 * build the actual layout (top-center timer, top-left TEAM CASH, top-right DETECTION,
 * bottom-center interaction prompt, bottom-left player status, optional noise meter) purely
 * in UMG. See Docs/UI_SPEC.md for the full layout spec and exact binding list.
 */
UCLASS()
class SUBURBANHEIST_API USHHUDWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HUD")
	FText FormatTime(float Seconds) const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// -- Bind these in WBP_HUD's Designer/Graph to drive the actual widgets --
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnTimeRemainingChanged(float SecondsRemaining);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnTeamCashChanged(float TeamMoney, float GoalMoney);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnDetectionChanged(int32 UniqueResidentsDetected, int32 DetectionLimit);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnNoiseLevelChanged(float NoiseLevel0to100);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnInteractionFocusChanged(AActor* FocusActor, const FText& Prompt);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnInteractionProgressChanged(float NormalizedProgress);

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGamePhaseChanged(EGamePhase NewPhase);

private:
	UFUNCTION() void HandleTimeChanged(float NewTimeRemaining);
	UFUNCTION() void HandleMoneyChanged(float NewTeamMoney);
	UFUNCTION() void HandleDetectionChanged(int32 NewUniqueResidentsDetected);
	UFUNCTION() void HandleNoiseChanged(float NewNoiseLevel);
	UFUNCTION() void HandlePhaseChanged(EGamePhase NewPhase);
};
