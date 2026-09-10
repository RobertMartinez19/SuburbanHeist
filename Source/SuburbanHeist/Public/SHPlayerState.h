#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SHPlayerState.generated.h"

/**
 * Per-player replicated stats (Section 19). Team-wide totals live on GameState;
 * this only tracks per-player contribution/status used for HUD flourishes and debug.
 */
UCLASS()
class SUBURBANHEIST_API ASHPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASHPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	float MoneyContributed = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	int32 ObjectivesCompleted = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsCurrentlyDetected = false;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void AddContribution(float MoneyAmount);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
