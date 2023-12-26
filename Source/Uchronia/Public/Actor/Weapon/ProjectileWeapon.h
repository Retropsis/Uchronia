// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

class UGameplayEffect;
class AAmmoContainer;
class AProjectile;
/**
 *  Weapon spawning a physical projectile, including rocket and grenade launchers 
 */
UCLASS()
class UCHRONIA_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Trigger(const FVector& HitTarget) override;

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	
private:
	/*
	 * Trace End with Scatter
	 */
	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties|Scatter")
	bool bUseScatter = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties|Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties|Scatter")
	float SphereRadius = 75.f;
};
