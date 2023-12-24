// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/BaseGameplayAbility.h"
#include "SummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCHRONIA_API USummonAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UPROPERTY(EditDefaultsOnly, Category="Ability|Summoning")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Summoning")
	float MinSpawnDistance = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Summoning")
	float MaxSpawnDistance = 400.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Summoning")
	float SpawnSpread = 90.f;
};
