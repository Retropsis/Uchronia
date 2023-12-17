// Retropsis @ 2023-2024


#include "AbilitySystem/BaseAbilitySystemComponent.h"

#include "BaseGameplayTags.h"

void UBaseAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UBaseAbilitySystemComponent::EffectApplied);

	const FBaseGameplayTags& GameplayTags = FBaseGameplayTags::Get();
	GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Emerald, FString::Printf(TEXT("%s"), *GameplayTags.Attributes_Secondary_Armor.ToString()));
}

void UBaseAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		// TODO: This is what I need to do on Server (HasAuthority) if i need to have Weapon as abilities
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		//GiveAbility(AbilitySpec);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UBaseAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
