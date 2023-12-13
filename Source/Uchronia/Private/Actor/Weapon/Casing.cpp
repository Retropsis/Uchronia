// Retropsis @ 2023-2024


#include "Actor/Weapon/Casing.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

