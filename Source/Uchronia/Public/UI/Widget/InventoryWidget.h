// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseUserWidget.h"
#include "InventoryWidget.generated.h"

class UWrapBox;
class UTextBlock;
class APlayerCharacter;
class UInventorySlot;
class UInventoryComponent_v4;
/**
 * 
 */
UCLASS()
class UCHRONIA_API UInventoryWidget : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RefreshInventory();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> InventoryWrapBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeightInfo;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CapacityInfo;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UInventoryComponent_v4> InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySlot> InventorySlotClass;
	
protected:
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};