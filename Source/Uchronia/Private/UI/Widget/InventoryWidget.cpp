// Retropsis @ 2023-2024


#include "UI/Widget/InventoryWidget.h"

#include "ActorComponents/Inventory/InventoryComponent_v4.h"
#include "Character/PlayerCharacter.h"
#include "Components/WrapBox.h"
#include "UI/Widget/InventorySlot.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory_V4();
		if(InventoryReference)
		{
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryWidget::RefreshInventory);
			// SetInfoText();
		}
	}
}

void UInventoryWidget::RefreshInventory()
{
	checkf(InventorySlotClass, TEXT("InventorySlotClass is missing, please fill up InventoryPanel!"));
	if(InventoryReference)
	{
		InventoryWrapBox->ClearChildren();
		for(const TSubclassOf<AW_Item> InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventorySlot* ItemSlot = CreateWidget<UInventorySlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);
			InventoryWrapBox->AddChildToWrapBox(ItemSlot);
		}
		SetInfoText();
	}
}

void UInventoryWidget::SetInfoText() const
{
	// const FString WeightInfoValue = { FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight())  + "/" + FString::SanitizeFloat(InventoryReference->GetWeightCapacity()) };
	// WeightInfo->SetText(FText::FromString(WeightInfoValue));
	//
	// const FString CapacityInfoValue = { FString::FromInt(InventoryReference->GetInventoryContents().Num())  + "/" + FString::FromInt(InventoryReference->GetSlotsCapacity()) };
	// CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}


bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
