// Retropsis @ 2023-2024


#include "Actor/Weapon/Rocket.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UchroniaBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void ARocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();
	if(FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if(FiringController)
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
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
