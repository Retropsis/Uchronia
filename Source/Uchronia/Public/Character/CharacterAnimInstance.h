// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

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

	UPROPERTY(BlueprintReadOnly, Category="CharacterMovement")
	bool bIsCrouched = false;
};
