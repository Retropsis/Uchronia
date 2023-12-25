// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/Projectile.h"
#include "Rocket.generated.h"

class URocketMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class UCHRONIA_API ARocket : public AProjectile
{
	GENERATED_BODY()

public:
	ARocket();
	virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void DestroyTimeEnd();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URocketMovementComponent> RocketMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	float DamageOuterRadius = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	TObjectPtr<UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	FName TrailSocketName = FName("TrailSystemSocket");

	FTimerHandle DestroyTimer;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	float DestroyTime = 3.f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	TObjectPtr<USoundBase> ProjectileLoopSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopComponent;

	UPROPERTY(EditDefaultsOnly, Category="Projectile Properties")
	TObjectPtr<USoundAttenuation> ProjectileLoopAttenuation;
};
