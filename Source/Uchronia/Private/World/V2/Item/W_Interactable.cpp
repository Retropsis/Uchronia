// Retropsis @ 2023-2024

#include "World/V2/Item/W_Interactable.h"
#include "Character/PlayerCharacter.h"

AW_Interactable::AW_Interactable()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	InteractableMesh->SetupAttachment(GetRootComponent());
	InteractableMesh->SetSimulatePhysics(true);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InteractableMesh->SetCollisionResponseToAllChannels(ECR_Block);
	InteractableMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractableMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
}

void AW_Interactable::BeginPlay()
{
	Super::BeginPlay();
}

void AW_Interactable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AW_Interactable::Interact(APlayerCharacter* PlayerCharacter)
{
	// 
}

