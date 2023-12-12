// Retropsis @ 2023-2024

#include "ActorComponents/CombatComponent.h"
#include "Actor/Weapon/Weapon.h"
#include "Character/CharacterAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/*
 * Combat Functionality
 */
void UCombatComponent::SetAiming(const bool IsAiming)
{
	bAiming = IsAiming;
	ServerSetAiming(IsAiming);
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::Trigger(const bool bPressed)
{
	bTriggerButtonPressed = bPressed;
	if(bTriggerButtonPressed) ServerTrigger();
}

void UCombatComponent::ServerTrigger_Implementation()
{
	MulticastTrigger();
}

void UCombatComponent::MulticastTrigger_Implementation()
{
	CharacterAnimInstance = CharacterAnimInstance ? CharacterAnimInstance : Cast<UCharacterAnimInstance>(PlayerCharacter->GetAnimInstance());
	if(EquippedWeapon == nullptr) return;
	if(IsValid(PlayerCharacter) && CharacterAnimInstance)
	{
		CharacterAnimInstance->PlayFireMontage(bAiming);
		EquippedWeapon->Trigger();
	}
}

void UCombatComponent::ServerSetAiming_Implementation(const bool IsAiming)
{
	bAiming = IsAiming;
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(IsValid(EquippedWeapon) && IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(PlayerCharacter == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* RightHandSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(PlayerCharacter); // is replicated

	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->bUseControllerRotationYaw = true;
}

