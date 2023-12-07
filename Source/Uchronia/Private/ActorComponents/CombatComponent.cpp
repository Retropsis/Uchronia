// Retropsis @ 2023-2024

#include "ActorComponents/CombatComponent.h"
#include "Actor/Weapon/Weapon.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
}

