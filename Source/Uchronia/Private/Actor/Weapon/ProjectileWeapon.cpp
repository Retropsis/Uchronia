// Retropsis @ 2023-2024

#include "Actor/Weapon/ProjectileWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayTags.h"
#include "Actor/Weapon/AmmoContainer.h"
#include "Actor/Weapon/Projectile.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

// void AProjectileWeapon::Trigger(const FVector& HitTarget)
// {
// 	Super::Trigger(HitTarget);
//
// 	if(!HasAuthority()) return;
// 	
// 	APawn* InstigatorPawn = Cast<APlayerCharacter>(GetOwner());
// 	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
// 	if(MuzzleFlashSocket)
// 	{
// 		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
// 		const FVector ToTarget = HitTarget - SocketTransform.GetLocation(); // From MuzzleFlashSocket to ImpactLocation
// 		const FRotator TargetRotation = ToTarget.Rotation();
// 		// if(GetAmmoContainer()&& GetAmmoContainer()->GetProjectileClass() && InstigatorPawn)
// 		if(ProjectileClass && InstigatorPawn)
// 		{
// 			if(UWorld* World = GetWorld())
// 			{
// 				FActorSpawnParameters SpawnParams;
// 				SpawnParams.Owner = GetOwner();
// 				SpawnParams.Instigator = InstigatorPawn;
// 				World->SpawnActor<AProjectile>(
// 					// GetAmmoContainer()->GetProjectileClass(),
// 					ProjectileClass,
// 					SocketTransform.GetLocation(),
// 					TargetRotation,
// 					SpawnParams
// 				);
// 			}
// 		}
// 	}
// }

void AProjectileWeapon::Trigger(const FVector& HitTarget)
{
	Super::Trigger(HitTarget);

	if(!HasAuthority()) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(CombatInterface && MuzzleFlashSocket)
	{
		// TODO: Get MuzzleFlash from CombatInterface
		// const FVector SocketLocation = CombatInterface->GetCombatSocketLocation(); 
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTarget = HitTarget - SocketTransform.GetLocation(); // From MuzzleFlashSocket to ImpactLocation
		const FRotator TargetRotation = ToTarget.Rotation();
		// TODO: Can decide on pitch here
		// Rotation.Pitch = 0.f;

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketTransform.GetLocation());
		SpawnTransform.SetRotation(TargetRotation.Quaternion());
		
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwner(),
			Cast<APawn>(GetOwner()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, SourceASC->MakeEffectContext());

		const FBaseGameplayTags GameplayTags = FBaseGameplayTags::Get();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, 25.f);
		Projectile->DamageEffectSpecHandle = SpecHandle;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	
}