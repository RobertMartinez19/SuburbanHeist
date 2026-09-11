#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SHInteractableInterface.generated.h"

class ACharacter;

UINTERFACE(BlueprintType)
class SUBURBANHEIST_API USHInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Reusable interaction contract (Section 17). Implemented by ASHObjectivePoint and any
 * other world interactable (doors, drawers, decorative props that generate noise, etc).
 * Never use real criminal terminology in GetInteractionPrompt() - keep it abstract
 * ("[E] SEARCH", "[E] COLLECT") per Section 1/17.
 *
 * Default bodies are given inline (rather than in a separate .cpp) so implementers only need
 * to override what they actually change. Parameter is named InteractingCharacter, not
 * Instigator, because AActor already declares a member called Instigator - naming a parameter
 * the same thing on an AActor-derived override (e.g. ASHObjectivePoint) shadows it and is a
 * compile error (C4458) under this engine's warning level.
 */
class SUBURBANHEIST_API ISHInteractableInterface
{
	GENERATED_BODY()

public:
	/** Server calls this once CanInteract is true and (for hold/QTE types) the duration/QTE
	 *  has been satisfied. Client-predicted cosmetic feedback happens separately in Blueprint. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(ACharacter* InteractingCharacter);
	virtual void Interact_Implementation(ACharacter* InteractingCharacter) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(ACharacter* InteractingCharacter) const;
	virtual bool CanInteract_Implementation(ACharacter* InteractingCharacter) const { return true; }

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt() const;
	virtual FText GetInteractionPrompt_Implementation() const
	{
		return NSLOCTEXT("SuburbanHeist", "DefaultInteractPrompt", "[E] INTERACT");
	}

	/** 0 for an instant interaction; >0 drives ASHInteractionComponent's hold-to-interact bar. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	float GetInteractionDuration() const;
	virtual float GetInteractionDuration_Implementation() const { return 0.f; }
};
