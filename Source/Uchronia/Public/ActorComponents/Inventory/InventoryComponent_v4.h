// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/Widget/BaseUserWidget.h"
#include "InventoryComponent_v4.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdatedSignature);

class AW_Item;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCHRONIA_API UInventoryComponent_v4 : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent_v4();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnInventoryUpdatedSignature OnInventoryUpdated;
	
	void ToggleInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateInventory(const TArray<TSubclassOf<AW_Item>>& InventoryToUpdate);

	UFUNCTION(Server, Reliable)
	void ServerAddInventoryItem(AW_Item* ItemToAdd);
	void AddInventoryItem(AW_Item* ItemToAdd);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSpawnItem(TSubclassOf<AW_Item> ItemClassToDrop, const FTransform& SpawnTransform);

	UPROPERTY(EditAnywhere, Category="Inventory | Widgets")
	TSubclassOf<UBaseUserWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UBaseUserWidget> InventoryWidget;

	UPROPERTY(ReplicatedUsing=OnRep_InventoryContents)
	TArray<TSubclassOf<AW_Item>> InventoryContents;

	UFUNCTION()
	void OnRep_InventoryContents();

	UFUNCTION(Server, Unreliable)
	void ServerPlayPickupSound(USoundBase* PickupSound);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayPickupSound(USoundBase* PickupSound);

protected:
	virtual void BeginPlay() override;
public:
	FORCEINLINE TArray<TSubclassOf<AW_Item>> GetInventoryContents() const { return InventoryContents; };
};
