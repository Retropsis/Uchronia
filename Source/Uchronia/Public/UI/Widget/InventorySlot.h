// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseUserWidget.h"
#include "InventorySlot.generated.h"

class UTextBlock;
class UImage;
class AW_Item;
/**
 * 
 */
UCLASS()
class UCHRONIA_API UInventorySlot : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetItemReference(TSubclassOf<AW_Item> Item) { ItemReference = Item; };
	FORCEINLINE TSubclassOf<AW_Item> GetItemReference() const { return ItemReference; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	// UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	// TSubclassOf<UDragItemVisual> DragItemVisualClass;
	
	// UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	// TSubclassOf<UInventoryToolTip> InventoryToolTipClass;
	
	UPROPERTY(VisibleAnywhere, Category="Inventory Slot")
	TSubclassOf<AW_Item> ItemReference;
	
	UPROPERTY(VisibleAnywhere, Category="Inventory Slot", meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;
	
	UPROPERTY(VisibleAnywhere, Category="Inventory Slot", meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemQuantity;
	
};
