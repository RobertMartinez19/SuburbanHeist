#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SHLootManager.generated.h"

class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSHOnObjectiveCompleted, AActor*, ObjectivePoint, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnTeamMoneyChanged, float, NewTeamMoney);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnTeamReachedGoal);

/**
 * BP_LootManager (Section 13). Server-authoritative gateway for all money awards - never let
 * ASHObjectivePoint or anything else touch ASHGameState::TeamMoney directly (Section 18).
 */
UCLASS()
class SUBURBANHEIST_API ASHLootManager : public AActor
{
	GENERATED_BODY()

public:
	ASHLootManager();

	UPROPERTY(BlueprintAssignable, Category = "Loot|Events")
	FSHOnObjectiveCompleted OnObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Loot|Events")
	FSHOnTeamMoneyChanged OnMoneyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Loot|Events")
	FSHOnTeamReachedGoal OnTeamReachedGoal;

	/** Called by ASHObjectivePoint::Interact (server) and by debug GiveMoney. */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void Server_AwardMoney(float Amount, APlayerState* ContributingPlayer);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void Server_NotifyObjectiveCompleted(AActor* ObjectivePoint, float Value, APlayerState* ContributingPlayer);
};
