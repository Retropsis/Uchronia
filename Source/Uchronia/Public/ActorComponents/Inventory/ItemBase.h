// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Data/ItemData.h"
#include "ItemBase.generated.h"

class UInventoryComponent;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class UCHRONIA_API UItemBase : public UObject
{
	GENERATED_BODY()
	
public:
	// Item Properties
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category="ItemData", meta=(UIMin=1, UIMax=100)) int32 Quantity;
	
	UPROPERTY(EditAnywhere, Category="Item Data") FName ID;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemTextData TextData;
	UPROPERTY(EditAnywhere, Category="Item Data") EItemType ItemType;
	UPROPERTY(EditAnywhere, Category="Item Data") EItemCondition ItemCondition;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemAssetData AssetData;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemStatistics ItemStatistics;
	
	// Functions
	UItemBase();

	UFUNCTION(Category="Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * NumericData.Weight; };
	UFUNCTION(Category="Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; };
	UFUNCTION(Category="Item")
	FORCEINLINE bool IsItemFullStack() const { return Quantity == NumericData.MaxStackSize; };
	UFUNCTION(Category="Item")
	FORCEINLINE bool IsItemFullCharge() const { return Quantity == NumericData.MaxCharges; };

	void SetQuantity(int32 NewQuantity);

	virtual void Use(APlayerCharacter* PlayerCharacter);

protected:
	bool operator==(const FName& OtherID) const
	{
		return ID == OtherID;
	}
};
