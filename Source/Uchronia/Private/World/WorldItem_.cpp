// Retropsis @ 2023-2024

#include "World/WorldItem_.h"
#include "Character/PlayerCharacter.h"

void AWorldItem_::Interact(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->AddItem(this);
	// Destroy(); // this should be server somewhere
}

void AWorldItem_::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Player Interfacing works
	// if(IInteractionInterface* Interface = Cast<IInteractionInterface>(OtherActor))
	// {
	// 	Interface->AddItem(this);
	// 	Destroy();
	// }
}

void AWorldItem_::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { }

void AWorldItem_::SetQuantity(int32 NewQuantity)
{
	if(NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, ItemData.bIsStackable ? ItemData.MaxStackSize : 1);

		// if(OwningInventory)
		// {
		// 	if(Quantity <= 0)
		// 	{
		// 		OwningInventory->RemoveSingleInstanceOfItem(this);
		// 	}
		// }
	}
	
}
