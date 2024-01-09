// Retropsis @ 2023-2024


#include "UI/Widget/InventorySlot.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "World/V2/Item/W_Item.h"

void UInventorySlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	if(ItemReference)
	{
		ItemIcon->SetBrushFromTexture(ItemReference->GetDefaultObject<AW_Item>()->ItemIcon);
	}
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply.Unhandled();
}

void UInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// checkf(DragItemVisualClass, TEXT("DragItemVisualClass is missing, please fill up WBP_InventoryItemSlot"));
	// const TObjectPtr<UDragItemVisual> DragItemVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
	// DragItemVisual->ItemIcon->SetBrushFromTexture(ItemReference->AssetData.Icon);
	// DragItemVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
	//
	// ItemReference->NumericData.bIsStackable
	// 	? DragItemVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity))
	// 	:  DragItemVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	//
	// DragItemVisual->ItemCharges->SetPercent(0.5f); 
	//
	// UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
	// DragItemOperation->SourceItem = ItemReference;
	// DragItemOperation->SourceInventory = ItemReference->OwningInventory;
	// DragItemOperation->DefaultDragVisual = DragItemVisual;
	// DragItemOperation->Pivot = EDragPivot::MouseDown;
	//
	// OutOperation = DragItemOperation;
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
