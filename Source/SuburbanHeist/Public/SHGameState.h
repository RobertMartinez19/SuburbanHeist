#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SHTypes.h"
#include "SHGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnPhaseChanged, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnMoneyChanged, float, NewTeamMoney);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnDetectionChanged, int32, NewUniqueResidentsDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnTimeChanged, float, NewTimeRemaining);

/**
 * Replicated match state (Section 19). All fields here are server-authoritative;
 * clients only ever read them via replication/RepNotify, never write them directly.
 */
UCLASS()
class SUBURBANHEIST_API ASHGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASHGameState();

	UPROPERTY(ReplicatedUsing = OnRep_GamePhase, BlueprintReadOnly, Category = "Match")
	EGamePhase GamePhase = EGamePhase::Lobby;

	UPROPERTY(ReplicatedUsing = OnRep_MatchTimeRemaining, BlueprintReadOnly, Category = "Match")
	float MatchTimeRemaining = 1200.f;

	UPROPERTY(ReplicatedUsing = OnRep_TeamMoney, BlueprintReadOnly, Category = "Match")
	float TeamMoney = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_UniqueResidentsDetected, BlueprintReadOnly, Category = "Match")
	int32 UniqueResidentsDetected = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match")
	float GoalMoney = 10000.f;

	UPROPERTY(BlueprintReadOnly, Category = "Match")
	int32 DetectionLimit = 3;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnMoneyChanged OnMoneyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnDetectionChanged OnDetectionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FSHOnTimeChanged OnTimeChanged;

	UFUNCTION(BlueprintPure, Category = "Match")
	FORCEINLINE bool IsMatchActive() const { return GamePhase == EGamePhase::Active; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_GamePhase();
	UFUNCTION() void OnRep_MatchTimeRemaining();
	UFUNCTION() void OnRep_TeamMoney();
	UFUNCTION() void OnRep_UniqueResidentsDetected();

	friend class ASHGameMode;
	friend class ASHTeamDetectionManager;
	friend class ASHLootManager;
};
