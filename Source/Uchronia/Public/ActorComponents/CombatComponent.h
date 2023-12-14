// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/PlayerHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class APlayerHUD;
class ACharacterPlayerController;
class UCharacterAnimInstance;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCHRONIA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class APlayerCharacter; // TODO: Could be base character ?
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool IsAiming);
	void Trigger(bool bPressed);
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool IsAiming);

	UFUNCTION(Server, Reliable)
	void ServerTrigger(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTrigger(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	TObjectPtr<APlayerCharacter> PlayerCharacter;
	UPROPERTY()
	ACharacterPlayerController* CharacterPlayerController;
	UPROPERTY()
	APlayerHUD* PlayerHUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY()
	UCharacterAnimInstance* CharacterAnimInstance;

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	UPROPERTY(Replicated)
	bool bAiming = false;

	bool bTriggerButtonPressed;

	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float AimWalkSpeed = 450.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float TraceExtent = 100.f;

	/*
	 * HUD & Crosshair
	 */
	float CrosshairVelocityModifier;
	float CrosshairAirborneModifier;
	float CrosshairMarksmanModifier;
	float CrosshairRecoilModifier;

	FVector HitTarget;
	
	FHUDPackage HUDPackage;

	/*
	 * Marksman Mode
	 */
	// Set by Camera's default FoV
	float DefaultFOV;
	float CurrentFOV;

	/* TODO: To DataAsset */
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float MarksmanFOV = 30.f;

	/* TODO: To DataAsset */
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float MarksmanInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*
	 * Automatic Fire
	 */
	FTimerHandle FireIntervalTimer;

	bool bCanFire = true;

	void FireIntervalStart();
	void FireIntervalEnd();
	
public:	

		
};
