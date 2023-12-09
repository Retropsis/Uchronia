// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class UCHRONIA_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> CharacterContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> DropAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ThrowAction;
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Jump();
	void EquipButtonPressed();
	void DropButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed(const FInputActionValue& Value);
	void FireButtonPressed(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void ThrowButtonPressed(const FInputActionValue& Value);
};


