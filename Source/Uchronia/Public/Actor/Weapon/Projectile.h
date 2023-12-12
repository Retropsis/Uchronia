// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class UCHRONIA_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	/*
	 * TODO: Should be DataAsset
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> Tracer;
	/* End */
	
	TObjectPtr<UParticleSystemComponent> TracerComponent;

public:	

};
