// Retropsis @ 2023-2024

#include "Actor/Weapon/ProjectileWeapon.h"
#include "Actor/Weapon/AmmoContainer.h"
#include "Actor/Weapon/Projectile.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Trigger(const FVector& HitTarget)
{
	Super::Trigger(HitTarget);

	if(!HasAuthority()) return;
	
	APawn* InstigatorPawn = Cast<APlayerCharacter>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTarget = HitTarget - SocketTransform.GetLocation(); // From MuzzleFlashSocket to ImpactLocation
		const FRotator TargetRotation = ToTarget.Rotation();
		// if(GetAmmoContainer()&& GetAmmoContainer()->GetProjectileClass() && InstigatorPawn)
		if(ProjectileClass && InstigatorPawn)
		{
			if(UWorld* World = GetWorld())
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = InstigatorPawn;
				World->SpawnActor<AProjectile>(
					// GetAmmoContainer()->GetProjectileClass(),
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}
}
