// Retropsis @ 2023-2024


#include "Item/Pickup.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(OverlapRadius);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 80.f)); 

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority()) OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APickup::Destroyed()
{
	Super::Destroyed();
	if(IsValid(PickupSound)) UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}

