// Retropsis @ 2023-2024

#include "Actor/Weapon/Rocket.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UchroniaBlueprintFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
	if(!HasAuthority()) CollisionBox->OnComponentHit.AddDynamic(this, &ARocket::OnHit);
	
	if(TrailSystem)
	{
		// TODO: Could specify a socket name to be more precise
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			TrailSocketName,
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);
	}
	if(ProjectileLoopSound && ProjectileLoopAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoopSound,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			FRotator::ZeroRotator,
			EAttachLocation::KeepWorldPosition,
			false,
			1,1, 0,
			ProjectileLoopAttenuation,
			(USoundConcurrency*) nullptr,
			false);
	}
}

void ARocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse, const FHitResult& Hit)
{
	const APawn* FiringPawn = GetInstigator();
	if(FiringPawn && HasAuthority())
	{
		if(const AController* FiringController = FiringPawn->GetController())
		{
			TArray<AActor*> Overlaps;
			TArray<AActor*> ActorsToIgnore;
			// UGameplayStatics::ApplyRadialDamageWithFalloff() TODO: Function to check for full functionality
			UUchroniaBlueprintFunctionLibrary::GetLivePlayersWithinRadius(
				this,
				Overlaps,
				ActorsToIgnore,
				500.f,
				GetActorLocation()
			);
			// TODO: Apply FallOff, InnerRadius
			if(HasAuthority())
			{
				for(AActor* Overlap : Overlaps)
				{
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Overlap))
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
					}
				}
			}
		}
	}
	GetWorldTimerManager().SetTimer(DestroyTimer,this, &ARocket::DestroyTimeEnd, DestroyTime);
	
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if(IsValid(ImpactSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ProjectileMesh) ProjectileMesh->SetVisibility(false);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController()) TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	if(ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())  ProjectileLoopComponent->Stop();
}

void ARocket::DestroyTimeEnd()
{
	Destroy();
}

void ARocket::Destroyed()
{
}