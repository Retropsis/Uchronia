// Retropsis @ 2023-2024


#include "Actor/Item/WorldItem.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

