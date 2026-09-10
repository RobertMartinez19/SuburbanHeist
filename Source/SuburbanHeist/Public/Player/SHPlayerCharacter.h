#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SHTypes.h"
#include "SHPlayerCharacter.generated.h"

class UCameraComponent;
class USHStaminaComponent;
class USHInteractionComponent;
class USHMicrophoneNoiseComponent;
class USHHealthComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * First-person co-op player (Section 16). Movement is arcade-responsive; every noisy action
 * routes through Server_EmitNoise so the (server-only) ASHNoiseManager stays authoritative.
 * Enhanced Input assets (IMC_Default, IA_Move, ...) are binary and documented for manual/
 * Python creation in Docs/BLUEPRINT_CONSTRUCTION.md - assign them on BP_PlayerCharacter.
 */
UCLASS()
class SUBURBANHEIST_API ASHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASHPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USHStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USHInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USHMicrophoneNoiseComponent> MicrophoneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USHHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed = 650.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float CrouchSpeed = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	float JumpNoise = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	float SprintNoisePerSecond = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	float NoiseReportRadius = 1000.f;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJumpStarted();
	void HandleSprintStarted();
	void HandleSprintStopped();
	void HandleCrouchStarted();
	void HandleCrouchStopped();
	void HandleInteract();

	/** Client -> Server: report a discrete or continuous noise event (Section 7). */
	UFUNCTION(Server, Unreliable)
	void Server_EmitNoise(ENoiseSourceType SourceType, float Loudness);

private:
	float TimeSinceLastSprintNoise = 0.f;
};
