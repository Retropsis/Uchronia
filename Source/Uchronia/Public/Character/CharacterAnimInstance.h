// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/AnimationStates.h"
#include "CharacterAnimInstance.generated.h"

class AWeapon;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class UCHRONIA_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual  void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bAirborne = false;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bIsAccelerating = false;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bWeaponEquipped = false;

	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bIsCrouched = false;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bAiming = false;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	float YawOffset;

	UPROPERTY(EditDefaultsOnly, Category="CharacterMovement")
	float YawOffsetInterpSpeed = 6.f;
	
	UPROPERTY(EditDefaultsOnly, Category="CharacterMovement")
	float LeanInterpSpeed = 6.f;
	
	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	float Lean;
	
	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	float AO_Yaw;
	
	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	FTransform LeftHandTransform;
	
	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	ETurningInPlace TurningInPlace;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;
};
