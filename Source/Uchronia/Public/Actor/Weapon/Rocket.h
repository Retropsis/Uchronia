// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/Projectile.h"
#include "Rocket.generated.h"

/**
 * 
 */
UCLASS()
class UCHRONIA_API ARocket : public AProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	float DamageOuterRadius = 500.f;

};
