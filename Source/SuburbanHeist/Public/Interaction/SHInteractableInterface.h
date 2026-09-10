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
 */
class SUBURBANHEIST_API ISHInteractableInterface
{
	GENERATED_BODY()

public:
	/** Server calls this once CanInteract is true and (for hold/QTE types) the duration/QTE
	 *  has been satisfied. Client-predicted cosmetic feedback happens separately in Blueprint. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(ACharacter* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(ACharacter* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt() const;

	/** 0 for an instant interaction; >0 drives ASHInteractionComponent's hold-to-interact bar. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	float GetInteractionDuration() const;
};
