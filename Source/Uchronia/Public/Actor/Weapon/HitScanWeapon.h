// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UCHRONIA_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Trigger(const FVector& HitTarget) override;

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	
	// TODO: Really need to be in DataAsset + AmmoContainer
	// TODO: Might need to change to Niagara 
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<UParticleSystem> ImpactParticles;
	
	// TODO: This could be in FireMontage
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<UParticleSystem> MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<UParticleSystem> BeamParticles;

	// TODO: This could be in FireMontage
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<USoundBase> FireSound;
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<USoundBase> ImpactSound;

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