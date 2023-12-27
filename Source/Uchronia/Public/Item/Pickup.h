// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class USphereComponent;

UCLASS()
class UCHRONIA_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(EditAnywhere, Category="Pickup Properties")
	TObjectPtr<USphereComponent> OverlapSphere;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditAnywhere, Category="Pickup Properties")
	TObjectPtr<USoundBase> PickupSound;
	
	UPROPERTY(EditAnywhere, Category="Pickup Properties")
	float OverlapRadius = 100.f;

	
	
public:	

};
