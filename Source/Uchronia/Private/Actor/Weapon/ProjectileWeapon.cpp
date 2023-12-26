// Retropsis @ 2023-2024

#include "Actor/Weapon/ProjectileWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayTags.h"
#include "Actor/Weapon/AmmoContainer.h"
#include "Actor/Weapon/Projectile.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"

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
		FVector EndLocation = bUseScatter ? TraceEndWithScatter(SocketTransform.GetLocation(), HitTarget) : SocketTransform.GetLocation() + (HitTarget - SocketTransform.GetLocation())  * 1.25f;
		const FVector ToTarget = EndLocation - SocketTransform.GetLocation(); // From MuzzleFlashSocket to ImpactLocation
		// const FVector ToTarget = HitTarget - SocketTransform.GetLocation(); // From MuzzleFlashSocket to ImpactLocation
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

		for (TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(1.f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

FVector AProjectileWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, SphereRadius);
	FVector EndLocation = SphereCenter + RandomVector;
	FVector ToEndLocation = EndLocation - TraceStart;
	
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 5.f);
	DrawDebugSphere(GetWorld(), EndLocation, 4.f, 12, FColor::White, false, 5.f);
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size()), FColor::Cyan, false, 3.f);

	return FVector(TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size());
}

void AProjectileWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutTraceHit)
{
	if(UWorld* World = GetWorld())
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart)  * 1.25f;
		World->LineTraceSingleByChannel(OutTraceHit, TraceStart, End, ECC_Visibility);
		// TODO: Need a valid BeamEnd when hitting the sky
		FVector BeamEnd = End;
		if(OutTraceHit.bBlockingHit)
		{
			BeamEnd = OutTraceHit.ImpactPoint;
		}
		// if (BeamParticles)
		// {
		// 	if(UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator, true))
		// 	{
		// 		Beam->SetVectorParameter(FName("Target"), BeamEnd);
		// 	}
		// }
	}
}
