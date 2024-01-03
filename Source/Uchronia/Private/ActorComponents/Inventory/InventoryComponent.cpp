// Retropsis @ 2023-2024

#include "ActorComponents/Inventory/InventoryComponent.h"
#include "ActorComponents/Inventory/ItemBase.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* InItem) const
{
	if(InItem)
	{
		if(InventoryContents.Contains(InItem))
		{
			return InItem;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* InItem) const
{
	if(InItem)
	{
		if(const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(InItem))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* InItem) const
{
	if(const TArray<TObjectPtr<UItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate([&InItem] (const UItemBase* InventoryItem)
		{
			return InventoryItem->ID == InItem->ID && !InventoryItem->IsItemFullStack();
		}))
	{
		return *Result;
	}
	return nullptr;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* InItem, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / InItem->GetItemSingleWeight());
	if(WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemToRemove, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemToRemove->Quantity);
	ItemToRemove->SetQuantity(ItemToRemove->Quantity - ActualAmountToRemove);
	InventoryTotalWeight -= ActualAmountToRemove * ItemToRemove->GetItemSingleWeight();
	OnInventoryUpdated.Broadcast();
	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemToSplit, int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemToSplit, AmountToSplit);
		AddNewItem(ItemToSplit, AmountToSplit);
	}
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* ItemToAdd)
{
	if(GetOwner()) //GetOwnerRole() might need this one?
	{
		const int32 InitialRequestedAddAmount = ItemToAdd->Quantity;

		// Non Stackable Items
		if(!ItemToAdd->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(ItemToAdd, InitialRequestedAddAmount);
		}

		// Stackable Items
		const int32 StackableAmountAdded =	HandleStackableItems(ItemToAdd, InitialRequestedAddAmount);
		if(StackableAmountAdded == InitialRequestedAddAmount)
		{
			// return added all result
		}
		if(StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			// return added partial result
		}
		if(StackableAmountAdded <= 0)
		{
			// return added none result
		}
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* InItem, int32 RequestedAddAmount)
{
	// Check for valid weight
	if(FMath::IsNearlyZero(InItem->GetItemSingleWeight()) || InItem->GetItemStackWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."), InItem->TextData.Name));
	}

	// Check if new Item will overflow weight capacity
	if(InventoryTotalWeight + InItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add {0} to the inventory. Item would overflow weight capacity."), InItem->TextData.Name));
	}

	// Check if new Item will overflow slots capacity
	if(InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add {0} to the inventory. Inventory is full."), InItem->TextData.Name));
	}
	AddNewItem(InItem, RequestedAddAmount);
		return FItemAddResult::AddedAll(RequestedAddAmount, FText::Format(FText::FromString("{0} {1} was added to the inventory."), RequestedAddAmount, InItem->TextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* InItem, int32 RequestedAddAmount)
{
}

void UInventoryComponent::AddNewItem(UItemBase* Item, int32 AmountToAdd)
{
	UItemBase* NewItem;

	if(Item->bIsCopy || Item->bIsPickup)
	{
		// when item is already a copy or from world pickup
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// when item is splitting pr dragged to another inventory
		NewItem = Item->CreateItemCopy();
	}
	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}
