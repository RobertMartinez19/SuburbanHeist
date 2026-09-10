#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SHTypes.generated.h"

/** Overall match phase, replicated from GameState. */
UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Lobby,
	Countdown,
	Active,
	Victory,
	Defeat,
	PoliceGameOver
};

/** Resident AI state machine (Section 9). Drives a single shared Behavior Tree. */
UENUM(BlueprintType)
enum class EResidentState : uint8
{
	Idle,
	Suspicious,
	Investigating,
	Alert,
	Detected,
	Chase,
	Return
};

/** Data-only archetype selector; behavior differs by tuning values, not by class (Section 8). */
UENUM(BlueprintType)
enum class EResidentArchetype : uint8
{
	Calm,
	Alert,
	Sensitive,
	Patrol
};

/** Source of a noise event, used for decay/propagation and debug display (Section 7). */
UENUM(BlueprintType)
enum class ENoiseSourceType : uint8
{
	Footstep,
	Jump,
	Sprint,
	Throw,
	Break,
	Door,
	Voice,
	Other
};

/** Fictional objective categories (Section 12). */
UENUM(BlueprintType)
enum class EObjectiveCategory : uint8
{
	ValuableObject,
	HiddenCache,
	SafeObjective,
	ElectronicsObjective,
	Collectible,
	BonusObjective
};

/** How an objective is completed (Section 12) - always abstract, never a real bypass mechanic. */
UENUM(BlueprintType)
enum class EObjectiveInteractionType : uint8
{
	HoldToComplete,
	QuickTimeEvent,
	RequiresKeyItem
};

/** DataTable row: tuning for one objective category (Section 26). */
USTRUCT(BlueprintType)
struct SUBURBANHEIST_API FSHObjectiveDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	EObjectiveCategory Category = EObjectiveCategory::ValuableObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	EObjectiveInteractionType InteractionType = EObjectiveInteractionType::HoldToComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective", meta = (ClampMin = "0"))
	float MinValue = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective", meta = (ClampMin = "0"))
	float MaxValue = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective", meta = (ClampMin = "0"))
	float InteractionDurationSeconds = 3.f;

	/** Noise added (0-100 scale) once, when interaction completes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective", meta = (ClampMin = "0", ClampMax = "100"))
	float NoiseOnComplete = 15.f;

	/** Noise added continuously per second while interacting (high-risk objectives are louder). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective", meta = (ClampMin = "0"))
	float NoisePerSecondWhileInteracting = 0.f;
};

/** DataTable row: tuning for one resident archetype (Section 8, 26). */
USTRUCT(BlueprintType)
struct SUBURBANHEIST_API FSHResidentArchetypeDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident")
	EResidentArchetype Archetype = EResidentArchetype::Calm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Perception", meta = (ClampMin = "0"))
	float SightRadius = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Perception", meta = (ClampMin = "0"))
	float SightHalfAngleDegrees = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Perception", meta = (ClampMin = "0"))
	float HearingRadius = 1500.f;

	/** Noise value (0-100) below which this archetype ignores sounds entirely. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Perception", meta = (ClampMin = "0", ClampMax = "100"))
	float HearingNoiseThreshold = 25.f;

	/** Suspicion (0-100) required to move from Suspicious -> Investigating. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Suspicion", meta = (ClampMin = "0", ClampMax = "100"))
	float InvestigateThreshold = 40.f;

	/** Suspicion (0-100) required to move from Investigating/Alert -> Detected. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Suspicion", meta = (ClampMin = "0", ClampMax = "100"))
	float DetectionThreshold = 85.f;

	/** How fast suspicion decays per second when not actively fed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Suspicion", meta = (ClampMin = "0"))
	float SuspicionDecayPerSecond = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Movement", meta = (ClampMin = "0"))
	float WalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Movement", meta = (ClampMin = "0"))
	float InvestigateSpeed = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Movement", meta = (ClampMin = "0"))
	float ChaseSpeed = 550.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Chase", meta = (ClampMin = "0"))
	float MaxChaseDurationSeconds = 20.f;

	/** Whether this archetype can use the fictional ResidentAttack chase mechanic (Section 11). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resident|Chase")
	bool bCanAttack = true;
};

/** One noise event as broadcast to the NoiseManager (Section 7). */
USTRUCT(BlueprintType)
struct SUBURBANHEIST_API FSHNoiseEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float Loudness = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Radius = 500.f;

	UPROPERTY(BlueprintReadOnly)
	ENoiseSourceType SourceType = ENoiseSourceType::Other;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Instigator = nullptr;
};
