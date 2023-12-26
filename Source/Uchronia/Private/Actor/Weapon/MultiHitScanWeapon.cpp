// Retropsis @ 2023-2024

#include "Actor/Weapon/MultiHitScanWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayTags.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AMultiHitScanWeapon::Trigger(const FVector& HitTarget)
{
	AWeapon::Trigger(HitTarget);
	
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
