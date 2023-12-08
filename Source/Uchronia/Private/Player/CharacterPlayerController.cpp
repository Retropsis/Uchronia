// Retropsis @ 2023-2024


#include "Player/CharacterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "ActorComponents/CombatComponent.h"
#include "Character/PlayerCharacter.h"
#include "Input/UEnhancedInputComponent.h"

ACharacterPlayerController::ACharacterPlayerController()
{
	bReplicates = true;
}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(CharacterContext);

	if(UEnhancedInputLocalPlayerSubsystem*  Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(CharacterContext, 0);
	}
}

void ACharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::CrouchButtonPressed);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::EquipButtonPressed);	
	EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::DropButtonPressed);	
}

void ACharacterPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ACharacterPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACharacterPlayerController::Jump()
{
	// TODO: Make it Lazy Init
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		PlayerCharacter->Jump();
	}
}

void ACharacterPlayerController::EquipButtonPressed()
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		PlayerCharacter->EquipWeapon();
	}
}

void ACharacterPlayerController::DropButtonPressed()
{
	
}

void ACharacterPlayerController::CrouchButtonPressed()
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		if(PlayerCharacter->bIsCrouched)
		{
			PlayerCharacter->UnCrouch();
		}
		else
		{
			PlayerCharacter->Crouch();
		}
	}
}
