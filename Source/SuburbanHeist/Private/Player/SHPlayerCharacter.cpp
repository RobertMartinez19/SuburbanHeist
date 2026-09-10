#include "Player/SHPlayerCharacter.h"
#include "Player/SHStaminaComponent.h"
#include "Player/SHInteractionComponent.h"
#include "Player/SHMicrophoneNoiseComponent.h"
#include "Gameplay/SHNoiseManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"

ASHPlayerCharacter::ASHPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Attached to the capsule at eye height rather than a mesh socket, so it works before any
	// specific block-style character mesh is assigned. See Docs/BLUEPRINT_CONSTRUCTION.md for
	// how BP_PlayerCharacter hides the body mesh from its own owner (SetOwnerNoSee) so the
	// player doesn't see their own head clipping the camera.
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	StaminaComponent = CreateDefaultSubobject<USHStaminaComponent>(TEXT("StaminaComponent"));
	InteractionComponent = CreateDefaultSubobject<USHInteractionComponent>(TEXT("InteractionComponent"));
	MicrophoneComponent = CreateDefaultSubobject<USHMicrophoneNoiseComponent>(TEXT("MicrophoneComponent"));
}

void ASHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ASHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction) EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASHPlayerCharacter::HandleMove);
		if (LookAction) EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASHPlayerCharacter::HandleLook);
		if (JumpAction) EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ASHPlayerCharacter::HandleJumpStarted);
		if (SprintAction)
		{
			EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ASHPlayerCharacter::HandleSprintStarted);
			EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASHPlayerCharacter::HandleSprintStopped);
			EIC->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ASHPlayerCharacter::HandleSprintStopped);
		}
		if (CrouchAction)
		{
			EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ASHPlayerCharacter::HandleCrouchStarted);
			EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ASHPlayerCharacter::HandleCrouchStopped);
		}
		if (InteractAction) EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &ASHPlayerCharacter::HandleInteract);
	}
}

void ASHPlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MoveInput.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MoveInput.X);
}

void ASHPlayerCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASHPlayerCharacter::HandleJumpStarted()
{
	Jump();
	Server_EmitNoise(ENoiseSourceType::Jump, JumpNoise);
}

void ASHPlayerCharacter::HandleSprintStarted()
{
	if (StaminaComponent && StaminaComponent->CanSprint())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		StaminaComponent->SetSprinting(true);
	}
}

void ASHPlayerCharacter::HandleSprintStopped()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsCrouched ? CrouchSpeed : WalkSpeed;
	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
	}
}

void ASHPlayerCharacter::HandleCrouchStarted()
{
	Crouch();
}

void ASHPlayerCharacter::HandleCrouchStopped()
{
	UnCrouch();
}

void ASHPlayerCharacter::HandleInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginInteract();
	}
}

void ASHPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	Server_EmitNoise(ENoiseSourceType::Footstep, JumpNoise * 0.5f);
}

void ASHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bSprinting = GetCharacterMovement()->MaxWalkSpeed >= SprintSpeed && !GetVelocity().IsNearlyZero();
	if (bSprinting && IsLocallyControlled())
	{
		TimeSinceLastSprintNoise += DeltaTime;
		if (TimeSinceLastSprintNoise >= 1.f)
		{
			TimeSinceLastSprintNoise = 0.f;
			Server_EmitNoise(ENoiseSourceType::Sprint, SprintNoisePerSecond);
		}
	}
}

void ASHPlayerCharacter::Server_EmitNoise_Implementation(ENoiseSourceType SourceType, float Loudness)
{
	TArray<AActor*> Managers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASHNoiseManager::StaticClass(), Managers);
	if (Managers.Num() == 0)
	{
		return;
	}

	if (ASHNoiseManager* Noise = Cast<ASHNoiseManager>(Managers[0]))
	{
		FSHNoiseEvent Event;
		Event.Location = GetActorLocation();
		Event.Loudness = Loudness;
		Event.Radius = NoiseReportRadius;
		Event.SourceType = SourceType;
		Event.Instigator = this;
		Noise->Server_ReportNoise(Event);
	}
}
