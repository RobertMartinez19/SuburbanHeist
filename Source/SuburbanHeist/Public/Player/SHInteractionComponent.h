#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSHOnFocusChanged, AActor*, NewFocus, FText, Prompt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSHOnInteractProgress, float, NormalizedProgress);

/**
 * Reusable interaction component (Section 17). Traces for ISHInteractableInterface actors,
 * drives the [E] prompt + hold-to-interact bar, and forwards completion to the server.
 * Attach to ASHPlayerCharacter.
 */
UCLASS(ClassGroup = (SuburbanHeist), meta = (BlueprintSpawnableComponent))
class SUBURBANHEIST_API USHInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USHInteractionComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float TraceRange = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float TraceSphereRadius = 15.f;

	/** How often (seconds) we re-trace for a focus target. Not every tick, per Section 27. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float TraceInterval = 0.1f;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FSHOnFocusChanged OnFocusChanged;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FSHOnInteractProgress OnInteractProgress;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BeginInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CancelInteract();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusActor() const { return CurrentFocus.Get(); }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RefreshFocus();

	UFUNCTION(Server, Reliable)
	void Server_RequestCompleteInteraction(AActor* Target);

private:
	FTimerHandle TraceTimerHandle;
	TWeakObjectPtr<AActor> CurrentFocus;
	float HoldElapsed = 0.f;
	bool bIsHolding = false;
};
