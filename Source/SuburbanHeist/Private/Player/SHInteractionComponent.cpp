#include "Player/SHInteractionComponent.h"
#include "Interaction/SHInteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

USHInteractionComponent::USHInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void USHInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &USHInteractionComponent::RefreshFocus, TraceInterval, true);
	}
}

void USHInteractionComponent::RefreshFocus()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	UCameraComponent* Camera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * TraceRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	FHitResult Hit;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, Start, End, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(TraceSphereRadius), QueryParams);

	AActor* NewFocus = nullptr;
	if (bHit && Hit.GetActor() && Hit.GetActor()->Implements<USHInteractableInterface>())
	{
		if (ISHInteractableInterface::Execute_CanInteract(Hit.GetActor(), OwnerCharacter))
		{
			NewFocus = Hit.GetActor();
		}
	}

	if (NewFocus != CurrentFocus.Get())
	{
		if (bIsHolding)
		{
			CancelInteract();
		}
		CurrentFocus = NewFocus;
		const FText Prompt = NewFocus ? ISHInteractableInterface::Execute_GetInteractionPrompt(NewFocus) : FText::GetEmpty();
		OnFocusChanged.Broadcast(NewFocus, Prompt);
	}
}

void USHInteractionComponent::BeginInteract()
{
	AActor* Focus = CurrentFocus.Get();
	if (!Focus)
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !ISHInteractableInterface::Execute_CanInteract(Focus, OwnerCharacter))
	{
		return;
	}

	const float Duration = ISHInteractableInterface::Execute_GetInteractionDuration(Focus);
	if (Duration <= 0.f)
	{
		Server_RequestCompleteInteraction(Focus);
		return;
	}

	bIsHolding = true;
	HoldElapsed = 0.f;
	SetComponentTickEnabled(true);
}

void USHInteractionComponent::CancelInteract()
{
	bIsHolding = false;
	HoldElapsed = 0.f;
	OnInteractProgress.Broadcast(0.f);
	SetComponentTickEnabled(false);
}

void USHInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsHolding)
	{
		SetComponentTickEnabled(false);
		return;
	}

	AActor* Focus = CurrentFocus.Get();
	if (!Focus)
	{
		CancelInteract();
		return;
	}

	const float Duration = ISHInteractableInterface::Execute_GetInteractionDuration(Focus);
	HoldElapsed += DeltaTime;
	OnInteractProgress.Broadcast(Duration > 0.f ? FMath::Clamp(HoldElapsed / Duration, 0.f, 1.f) : 1.f);

	if (HoldElapsed >= Duration)
	{
		Server_RequestCompleteInteraction(Focus);
		CancelInteract();
	}
}

void USHInteractionComponent::Server_RequestCompleteInteraction_Implementation(AActor* Target)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !Target || !Target->Implements<USHInteractableInterface>())
	{
		return;
	}

	// Server re-validates range/CanInteract - never trust the client's claim (Section 18).
	const float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), Target->GetActorLocation());
	const float MaxAllowedDist = TraceRange + TraceSphereRadius + 50.f;
	if (DistSq > FMath::Square(MaxAllowedDist))
	{
		return;
	}

	if (ISHInteractableInterface::Execute_CanInteract(Target, OwnerCharacter))
	{
		ISHInteractableInterface::Execute_Interact(Target, OwnerCharacter);
	}
}
