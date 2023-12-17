// Retropsis @ 2023-2024


#include "Player/CharacterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Character/PlayerCharacter.h"
#include "GameplayTagContainer.h"
#include "Input/PlayerInputComponent.h"

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

	UPlayerInputComponent* PlayerInputComponent = CastChecked<UPlayerInputComponent>(InputComponent);

	PlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Move);
	PlayerInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Look);
	PlayerInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Jump);
	PlayerInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::CrouchButtonPressed);
	PlayerInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::EquipButtonPressed);	
	PlayerInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::DropButtonPressed);	
	PlayerInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::AimButtonPressed);	
	PlayerInputComponent->BindAction(TriggerPressedAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::TriggerButtonPressed);	
	PlayerInputComponent->BindAction(TriggerReleasedAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::TriggerButtonReleased);	
	PlayerInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::ThrowButtonPressed);	
	PlayerInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::ReloadButtonPressed);

	PlayerInputComponent->SetupKeybindInputActions(InputConfig, this, &ThisClass::KeybindInputTagPressed, &ThisClass::KeybindInputTagReleased, &ThisClass::KeybindInputTagHeld);
}

void ACharacterPlayerController::KeybindInputTagPressed(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *InputTag.ToString()));
}

void ACharacterPlayerController::KeybindInputTagReleased(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Red, FString::Printf(TEXT("%s"), *InputTag.ToString()));
}

void ACharacterPlayerController::KeybindInputTagHeld(FGameplayTag InputTag)
{
	GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Orange, FString::Printf(TEXT("%s"), *InputTag.ToString()));
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

void ACharacterPlayerController::AimButtonPressed(const FInputActionValue& Value)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		PlayerCharacter->Aim(Value.Get<bool>());
	}
}

void ACharacterPlayerController::TriggerButtonPressed(const FInputActionValue& Value)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		GEngine->AddOnScreenDebugMessage(3, 0, FColor::Green, FString::Printf(TEXT("%s"), Value.Get<bool>() ? TEXT("TriggerButtonPressed is true") : TEXT("TriggerButtonPressed is false")));
		PlayerCharacter->TriggerButtonPressed(Value.Get<bool>());
	}
}

void ACharacterPlayerController::TriggerButtonReleased(const FInputActionValue& Value)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter()))
	{
		GEngine->AddOnScreenDebugMessage(4, 0, FColor::Red, FString::Printf(TEXT("%s"), Value.Get<bool>() ? TEXT("TriggerButtonReleased is true") : TEXT("TriggerButtonReleased is false")));
		PlayerCharacter->TriggerButtonReleased(Value.Get<bool>());
	}
}

void ACharacterPlayerController::ReloadButtonPressed(const FInputActionValue& Value)
{
	
}

void ACharacterPlayerController::ThrowButtonPressed(const FInputActionValue& Value)
{
	
}
