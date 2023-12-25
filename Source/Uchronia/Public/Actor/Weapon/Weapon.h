// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ACharacterPlayerController;
class APlayerCharacter;
class ACasing;
class AAmmoContainer;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),

	EWS_MAX UMETA(DisplayName="DefaultMAX"),
};

class USphereComponent;

UCLASS()
class UCHRONIA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(const bool bShowWidget) const;
	virtual void Trigger(const FVector& HitTarget);
	virtual void Drop();
	virtual void OnRep_Owner() override;
	void AddRounds(int32 RoundsToAdd);
	void SetHUDAmmo();
	
	/*
	 * TODO: Move both CH and FoV stuff to Weapon DataAsset
	*/
	/*
	 * Crosshair Textures
	 */
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	TObjectPtr<UTexture2D> Crosshair_Center;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	TObjectPtr<UTexture2D> Crosshair_Left;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	TObjectPtr<UTexture2D> Crosshair_Top;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	TObjectPtr<UTexture2D> Crosshair_Right;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	TObjectPtr<UTexture2D> Crosshair_Bottom;

	/*
	 * Automatic Fire
	 */
	/* TODO: To DataAsset */
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	float FireInterval = .15f;

	/* TODO: To DataAsset */
	UPROPERTY(EditDefaultsOnly, Category="Combat Properties")
	bool bAutomatic = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual  void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);

	UFUNCTION()
	virtual  void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY()
	APlayerCharacter* OwnerCharacter;
	
	UPROPERTY()
	ACharacterPlayerController* OwnerController;
	
	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category="Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	/*
	 * TODO: Move to DataAsset
	*/
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<AAmmoContainer> AmmoContainer;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<ACasing> CasingClass;

	// TODO: Should be its own container class
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity = 8;

	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType;
	/*  */

	/*
	 * Marksman Mode
	 */
	UPROPERTY(EditAnywhere, Category="Marksman Mode")
	float MarksmanFOV = 30.f;

	UPROPERTY(EditAnywhere, Category="Marksman Mode")
	float MarksmanInterpSpeed = 20.f;
	/*
	 * End
	*/
	
public:
	void SetWeaponState(const EWeaponState InWeaponState);
	FORCEINLINE USphereComponent* GetOverlapSphere() const { return OverlapSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE AAmmoContainer* GetAmmoContainer() const { return AmmoContainer; }
	FORCEINLINE float GetMarksmanFOV() const { return MarksmanFOV; }
	FORCEINLINE float GetMarksmanInterpSpeed() const { return MarksmanInterpSpeed; }
	FORCEINLINE bool HasAmmo() const { return Ammo > 0; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; } 
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; } 
};
