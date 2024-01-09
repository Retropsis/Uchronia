// Retropsis @ 2023-2024


#include "World/V2/Item/W_Item.h"

#include "ActorComponents/Inventory/InventoryComponent_v4.h"
#include "Character/PlayerCharacter.h"

void AW_Item::Interact(APlayerCharacter* PlayerCharacter)
{
	if(!IsValid(PlayerCharacter)) return;
	if(HasAuthority())
	{
		if(PlayerCharacter->InventoryComponent_V4)
		{
			PlayerCharacter->InventoryComponent_V4->AddInventoryItem(this);
			Destroy();
		}
	}
	else
	{
		PlayerCharacter->InventoryComponent_V4->ServerAddInventoryItem(this);
	}
}
