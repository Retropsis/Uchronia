// Retropsis @ 2023-2024


#include "ActorComponents/Inventory/InventoryComponent_v4.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/InventoryWidget.h"
#include "World/V2/Item/W_Item.h"

UInventoryComponent_v4::UInventoryComponent_v4()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent_v4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UInventoryComponent_v4, InventoryContents, COND_OwnerOnly);
}

void UInventoryComponent_v4::BeginPlay()
{
	Super::BeginPlay();	
}

void UInventoryComponent_v4::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent_v4::ToggleInventory()
{
	checkf(InventoryWidgetClass, TEXT("InventoryWidgetClass missing in InventoryComponent"));
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController == nullptr) return;
	
	if(InventoryWidget)
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
	}
	else
	{
        if (PlayerController->IsLocalPlayerController())
        {
	        UUserWidget* Widget = CreateWidget(PlayerController, InventoryWidgetClass, FName("Inventory Widget"));
	        InventoryWidget = Cast<UBaseUserWidget>(Widget);
	        Widget->AddToViewport(0);
	        OnInventoryUpdated.Broadcast();
	        FInputModeGameAndUI InputMode;
        	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	        PlayerController->SetInputMode(InputMode);
	        PlayerController->SetShowMouseCursor(true);
        }
	}
}

void UInventoryComponent_v4::ServerAddInventoryItem_Implementation(AW_Item* ItemToAdd)
{
	AddInventoryItem(ItemToAdd);
}

void UInventoryComponent_v4::AddInventoryItem(AW_Item* ItemToAdd)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController && PlayerController->HasAuthority())
	{
		bool bIsNewItem = true;
		// for(TSubclassOf<AW_Item>& Item : InventoryContents)
		// {
		// 	if(Item == ItemToAdd->GetClass())
		// 	{
		// 		if(ItemData.ItemQuantity > 1)
		// 		{
		// 			Item.ItemQuantity += ItemData.ItemQuantity;
		// 		}
		// 		else
		// 		{
		// 			++Item.ItemQuantity;
		// 		}
		// 		bIsNewItem = false;
		// 		break;
		// 	}
		// }
		if(bIsNewItem)
		{
			InventoryContents.Add(ItemToAdd->GetClass());
			ServerPlayPickupSound(ItemToAdd->PickupSound);
			// InventoryItems.Add(ItemData);
		}
		if(PlayerController->IsLocalPlayerController())
		{
			OnRep_InventoryContents();
		}
	}
}

void UInventoryComponent_v4::ServerSpawnItem_Implementation(TSubclassOf<AW_Item> ItemClassToDrop,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<AW_Item>(ItemClassToDrop, SpawnTransform, SpawnParameters);
}

void UInventoryComponent_v4::OnRep_InventoryContents()
{
	// UpdateInventory(InventoryContents);
	if (UInventoryWidget* Widget = Cast<UInventoryWidget>(InventoryWidget))
	{
		if (Widget)
		{
			Widget->RefreshInventory();
		}
	}
}

void UInventoryComponent_v4::ServerPlayPickupSound_Implementation(USoundBase* PickupSound)
{
	MulticastPlayPickupSound(PickupSound);
}

void UInventoryComponent_v4::MulticastPlayPickupSound_Implementation(USoundBase* PickupSound)
{
	if(PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetOwner()->GetActorLocation());
	}
}
