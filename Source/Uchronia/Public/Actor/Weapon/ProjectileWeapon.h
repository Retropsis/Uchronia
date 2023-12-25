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
};
