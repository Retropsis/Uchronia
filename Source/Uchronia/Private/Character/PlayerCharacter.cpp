// Retropsis @ 2023

#include "Character/PlayerCharacter.h"

#include "Actor/Weapon/Weapon.h"
#include "ActorComponents/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Display"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

/*
 * UEngine Functionality
 */
void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->PlayerCharacter = this;
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

/*
 *  Weapon Functionality
*/
void APlayerCharacter::EquipWeapon()
{
	if(CombatComponent)
	{
		if(HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void APlayerCharacter::Aim(const bool bIsAiming)
{
	if(CombatComponent)
	{
		CombatComponent->SetAiming(bIsAiming);
	}	
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}	
}

void APlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(false);
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
	}
}

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) const
{
	if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (CombatComponent && CombatComponent->bAiming);
}

/*
*
*/
