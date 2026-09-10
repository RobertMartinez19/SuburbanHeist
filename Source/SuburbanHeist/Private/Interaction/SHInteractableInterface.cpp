#include "Interaction/SHInteractableInterface.h"

// Default BlueprintNativeEvent implementations - concrete interactables (e.g. ASHObjectivePoint)
// override these directly in C++; pure-Blueprint interactables implement them in the BP graph.

void ISHInteractableInterface::Interact_Implementation(ACharacter* Instigator)
{
}

bool ISHInteractableInterface::CanInteract_Implementation(ACharacter* Instigator) const
{
	return true;
}

FText ISHInteractableInterface::GetInteractionPrompt_Implementation() const
{
	return NSLOCTEXT("SuburbanHeist", "DefaultInteractPrompt", "[E] INTERACT");
}

float ISHInteractableInterface::GetInteractionDuration_Implementation() const
{
	return 0.f;
}
