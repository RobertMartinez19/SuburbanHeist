#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SHTeamDetectionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSHOnUniqueResidentDetected, AActor*, Resident, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSHOnThirdResidentDetected);

/**
 * BP_TeamDetectionManager (Section 5). Server-authoritative. Deduplicates detections by
 * resident identity: the SAME resident detecting multiple players only counts once, but
 * DIFFERENT residents each count, exactly as specified. Mirrors its count into
 * ASHGameState::UniqueResidentsDetected (the value everything else, including HUD, reads).
 */
UCLASS()
class SUBURBANHEIST_API ASHTeamDetectionManager : public AActor
{
	GENERATED_BODY()

public:
	ASHTeamDetectionManager();

	UPROPERTY(BlueprintReadOnly, Category = "Detection")
	int32 DetectionLimit = 3;

	UPROPERTY(BlueprintAssignable, Category = "Detection|Events")
	FSHOnUniqueResidentDetected OnUniqueResidentDetected;

	UPROPERTY(BlueprintAssignable, Category = "Detection|Events")
	FSHOnThirdResidentDetected OnThirdResidentDetected;

	/** Called by ASHResidentAIController when a Resident's state reaches Detected (Section 9). */
	UFUNCTION(BlueprintCallable, Category = "Detection")
	void Server_ReportDetection(AActor* ResidentActor, AActor* DetectedPlayer);

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void Server_ResetAll();

	UFUNCTION(BlueprintPure, Category = "Detection")
	int32 GetUniqueResidentsDetectedCount() const { return DetectedResidents.Num(); }

	/** Section 24 TriggerDetection - bumps the shared counter without a real resident. */
	void Debug_ForceDetection();

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> DetectedResidents;

	void PushCountToGameState();
};
