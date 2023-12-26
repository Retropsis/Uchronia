// Retropsis @ 2023-2024

#include "Actor/Weapon/HitScanWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayTags.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Trigger(const FVector& HitTarget)
{
	Super::Trigger(HitTarget);

	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash")))
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start)  * 1.25f;

		if (UWorld* World = GetWorld())
		{
			FHitResult ScanHit;
			World->LineTraceSingleByChannel(ScanHit, Start, End, ECC_Visibility);

			// TODO: Need a valid BeamEnd when hitting the sky
			FVector BeamEnd = End;
			if (ScanHit.bBlockingHit)
			{
				BeamEnd = ScanHit.ImpactPoint;
				if (HasAuthority())
				{
                    const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
                    const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, SourceASC->MakeEffectContext());
    
                    const FBaseGameplayTags GameplayTags = FBaseGameplayTags::Get();
    
                    for (TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
                    {
                    	const float ScaledDamage = Pair.Value.GetValueAtLevel(1.f);
                    	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
                    }
                    if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ScanHit.GetActor()))
                    {
                    	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                    }
				}
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, ScanHit.ImpactPoint, ScanHit.ImpactNormal.Rotation());
				}
				if(ImpactSound)
				{
					UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, ScanHit.ImpactPoint);
				}
				if (BeamParticles)
				{
					if(UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform))
					{
						Beam->SetVectorParameter(FName("Target"), BeamEnd);
					}
				}
			}
			if(MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
			}
			if(FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, FireSound, SocketTransform.GetLocation());
			}
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
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