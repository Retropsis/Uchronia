// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractable.h"
#include "GameFramework/Actor.h"
#include "WorldItem.generated.h"

class UInventoryComponent;

UCLASS()
class UCHRONIA_API AWorldItem : public AWorldInteractable
{
	GENERATED_BODY()
	
public:	
	AWorldItem();
	
	void InitializePickup(const TSubclassOf<AWorldItem> BaseClass, const int32 InQuantity);
	void InitializeDrop(AWorldItem* ItemToDrop, const int32 InQuantity);
	
	virtual void UpdateInteractableData() override;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> OverlappingInventory;
	
	UPROPERTY(EditInstanceOnly, Category="Pickup Properties | Initialization")
	int32 ItemQuantity;
	
	UPROPERTY(EditInstanceOnly, Category="Pickup Properties | Initialization")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditInstanceOnly, Category="Pickup Properties | Initialization")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(VisibleAnywhere, Category="Pickup Properties")
	TObjectPtr<AWorldItem> ItemReference;

protected:
	virtual void BeginPlay() override;
	
};
