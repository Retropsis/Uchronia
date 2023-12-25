// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
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
	// virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
	/*
	 * TODO: Should be DataAsset
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// TODO: Might need to change to Niagara 
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

private:

	/*
	 * TODO: Should be DataAsset
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> Tracer;
	/* End */
	
	TObjectPtr<UParticleSystemComponent> TracerComponent;

public:	

};
