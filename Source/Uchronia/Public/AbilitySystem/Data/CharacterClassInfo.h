// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Survivor,
	Mariner,
	LoneHunter,
	PackHunter,
	Worker,
	Soldier,
	Vulture,
	Critter,
	Elite,
	Leviathan,
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	
};

/**
 * 
 */
UCLASS()
class UCHRONIA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassDefaultInformation;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
