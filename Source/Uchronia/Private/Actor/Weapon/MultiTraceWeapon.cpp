// Retropsis @ 2023-2024

#include "Actor/Weapon/MultiTraceWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayTags.h"
#include "Actor/Weapon/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interaction/CombatInterface.h"

void AMultiTraceWeapon::Trigger(const FVector& HitTarget)
{
	AWeapon::Trigger(HitTarget);
	
	if(!HasAuthority()) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	
	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")))
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		
		for(uint32 i = 0; i < NumHits; i++)
		{
			FVector End = TraceEndWithScatter(Start, HitTarget);
		}
	}
}
