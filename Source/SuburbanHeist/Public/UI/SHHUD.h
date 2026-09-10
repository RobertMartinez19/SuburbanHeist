#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SHHUD.generated.h"

class USHHUDWidgetBase;

/** Creates and owns the single WBP_HUD instance (Section 20) for the local player. */
UCLASS()
class SUBURBANHEIST_API ASHHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<USHHUDWidgetBase> HUDWidgetClass;

	UFUNCTION(BlueprintPure, Category = "HUD")
	USHHUDWidgetBase* GetHUDWidget() const { return HUDWidgetInstance; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<USHHUDWidgetBase> HUDWidgetInstance;
};
