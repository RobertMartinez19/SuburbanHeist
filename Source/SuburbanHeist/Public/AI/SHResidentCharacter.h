#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SHTypes.h"
#include "SHResidentCharacter.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSHOnResidentStateChanged, EResidentState, OldState, EResidentState, NewState);

/**
 * Base class for every Resident. BP_Resident_Calm/Alert/Sensitive/Patrol are DATA-ONLY
 * Blueprint children of this class (Section 8) - they differ only by which
 * FSHResidentArchetypeDataRow they point to, never by unique logic. All state-machine logic
 * (Section 9) lives here in C++, server-authoritative; ASHResidentAIController and the BT
 * nodes only read/write Blackboard mirrors and call these Server_ functions.
 */
UCLASS()
class SUBURBANHEIST_API ASHResidentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASHResidentCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident")
	EResidentArchetype Archetype = EResidentArchetype::Calm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident")
	TObjectPtr<UDataTable> ArchetypeDataTable;

	/** Row lookup key. Defaults to the Archetype enum's display name; override per-instance if needed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident")
	FName ArchetypeRowNameOverride;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentState, BlueprintReadOnly, Category = "Resident")
	EResidentState CurrentState = EResidentState::Idle;

	UPROPERTY(ReplicatedUsing = OnRep_SuspicionLevel, BlueprintReadOnly, Category = "Resident")
	float SuspicionLevel = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Resident")
	FVector HomeLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Resident")
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY(BlueprintReadOnly, Category = "Resident")
	FVector LastHeardLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintAssignable, Category = "Resident|Events")
	FSHOnResidentStateChanged OnStateChanged;

	UFUNCTION(BlueprintPure, Category = "Resident")
	const FSHResidentArchetypeDataRow& GetTuning() const { return Tuning; }

	UFUNCTION(BlueprintCallable, Category = "Resident")
	void SetArchetypeByName(const FString& ArchetypeName);

	UFUNCTION(BlueprintCallable, Category = "Resident")
	void Server_AddSuspicion(float Delta, AActor* SourceActor, FVector SourceLocation);

	UFUNCTION(BlueprintCallable, Category = "Resident")
	void Server_TickSuspicionDecay(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Resident")
	void Server_SetState(EResidentState NewState);

	/** Fictional chase attack (Section 11) - abstract gameplay only, no real weapon simulation. */
	UFUNCTION(BlueprintCallable, Category = "Resident|Attack")
	void Server_PerformChaseAttack(AActor* Target);

	/** Starts the attack cooldown regardless of hit/miss - call once a telegraph resolves. */
	UFUNCTION(BlueprintCallable, Category = "Resident|Attack")
	void Server_MarkAttackAttempted();

	/** Fires the telegraph cue immediately, giving the player a window to escape (Section 11)
	 *  before the attack actually resolves. */
	UFUNCTION(BlueprintCallable, Category = "Resident|Attack")
	void Server_BeginAttackTelegraph(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Resident|Attack")
	bool CanAttack() const { return Tuning.bCanAttack; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resident|Attack")
	float ChaseAttackDamage = 10.f;

	/** Server-only cooldown bookkeeping for UBTTask_ResidentAttack; world seconds of last attack. */
	UPROPERTY(BlueprintReadOnly, Category = "Resident|Attack")
	float LastAttackWorldTime = -1000.f;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION() void OnRep_CurrentState();
	UFUNCTION() void OnRep_SuspicionLevel();

	/** Bind in a Blueprint child for a telegraph animation/VFX before damage lands. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Resident|Attack")
	void OnAttackTelegraph(AActor* Target);

	FSHResidentArchetypeDataRow Tuning;

private:
	void LoadTuning();
	void ApplyMovementSpeedForState(EResidentState State);

	float ChaseElapsedSeconds = 0.f;
};
