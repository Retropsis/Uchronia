// Retropsis @ 2023

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Character/BaseCharacter.h"
#include "Types/AnimationStates.h"
#include "PlayerCharacter.generated.h"

class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class UCHRONIA_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_ReplicatedMovement() override;

	virtual void Jump() override;
	void EquipWeapon();
	void Aim(bool bIsAiming);
	void TriggerButtonPressed(bool bPressed);
	void TriggerButtonReleased(bool bPressed);
	
	//~ Combat Interface 
	virtual void HitReact() override;
	//~ Combat Interface

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitReact();

protected:
	// TODO: Could be somewhere else like WidgetController
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> OverheadWidget;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	// TODO: Could be any pickable actor
	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;

	/*
	 * Combat
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> CombatComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float CalculateGroundSpeed();
	void CalculateAO_Pitch();
	void AimOffset(float DeltaTime);
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float InterpAO_YawSpeed = 4.f;
	
	/*
	 * Turning In Place
	 */
	ETurningInPlace TurningInPlace = ETurningInPlace::ETIP_None;
	void TurnInPlace(float DeltaTime);
	
	void SimProxiesTurn();
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	
	/*
	 * Camera
	 */
	void HideCharacterIfCameraClose();

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraThreshold = 50.f;

	/*
	 * Ability System
	 */
	virtual void InitAbilityActorInfo() override;
		
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	AWeapon* GetEquippedWeapon();
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; };
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	bool IsWeaponEquipped() const;
	bool IsAiming() const;

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; };
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; };
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	
	UAnimInstance* GetAnimInstance() const;
	FVector GetHitTarget() const;

};


