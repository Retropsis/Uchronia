// Retropsis @ 2023-2024

#include "Actor/Weapon/Weapon.h"

#include "Actor/Weapon/AmmoContainer.h"
#include "Actor/Weapon/Casing.h"
#include "ActorComponents/CombatComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/CharacterPlayerController.h"

/*
 * This should be the base weapon and not used as BP,
 * Holding all properties and functionality for constructing a basic weapon
 */

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetRootComponent());
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// WeaponMesh->SetSimulatePhysics(true);
	// WeaponMesh->SetEnableGravity(true);
	// WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OverlapSphere->SetSphereRadius(OverlapSphereRadius);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawAtDesiredSize(true);
	
	AmmoContainer = CreateDefaultSubobject<AAmmoContainer>(TEXT("Ammo Container"));
	const USkeletalMeshSocket* ClipSocket =WeaponMesh->GetSocketByName(FName("ClipSocket"));
	if(ClipSocket)
	{
		ClipSocket->AttachActor(AmmoContainer, WeaponMesh);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
		OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	if (PickupWidget) PickupWidget->SetVisibility(false);
}

void AWeapon::Trigger(const FVector& HitTarget)
{
	if(IsValid(FireAnimation))
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if(CasingClass)
	{
		if(const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject")))
		{
			const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			if(UWorld* World = GetWorld())
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	SpendRound();
}

void AWeapon::Drop()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr); 
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::AddRounds(int32 RoundsToAdd)
{
	Ammo = FMath::Clamp(Ammo + RoundsToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::OnRep_Ammo()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%d"), Ammo), true, true, FLinearColor::Blue, 3.f);
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<APlayerCharacter>(GetOwner()) : OwnerCharacter;
	if(IsValid(OwnerCharacter) && OwnerCharacter->GetCombatComponent() && IsFull())
	{
		OwnerCharacter->GetCombatComponent()->JumpToReloadEnd();
	}
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if(Owner == nullptr)
	{
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetHUDAmmo()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<APlayerCharacter>(GetOwner()) : OwnerCharacter;
	if(OwnerCharacter)
	{
		OwnerController = OwnerController == nullptr ? Cast<ACharacterPlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController)
		{
			OwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::ShowPickupWidget(const bool bShowWidget) const
{
	if(PickupWidget) PickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::SetWeaponState(const EWeaponState InWeaponState)
{
	WeaponState = InWeaponState;
	switch (WeaponState) {
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if(WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		break;
	case EWeaponState::EWS_Dropped:
		if(HasAuthority()) OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		break;
	default: ;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState) {
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		// OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if(WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		break;
	default: ;
	}
}
