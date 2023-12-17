// Retropsis @ 2023-2024


#include "BaseGameplayTags.h"
#include "GameplayTagsManager.h"

FBaseGameplayTags FBaseGameplayTags::GameplayTags;

void FBaseGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"), FString("Physical damage, carry weight, lifting"));
	GameplayTags.Attributes_Primary_Toughness = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Toughness"), FString("Physical resistance, health and carry weight"));
	GameplayTags.Attributes_Primary_Dexterity = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Dexterity"), FString("Skill in performing tasks, especially with hands"));
	GameplayTags.Attributes_Primary_Perception = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Perception"), FString("Ability to see, hear or become aware of something through the senses"));

	/*
	 * Secondary Attributes
	 */
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduces damage taken and improves block chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("Armor reduction"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("Chance to block a hit"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"), FString("Chance to apply a critical hit"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"), FString("Critical damage amount"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"), FString("Reduces critical damage"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"), FString("Amount of health points regenerated periodically"));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("Amount of maximum health points"));

	/*
	 * Vital Attributes
	 */
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"), FString("Amount of health points"));
	
	/*
	 * Input Tags for Keybinding
	 */
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"), FString("Amount of health points"));
	
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"), FString("InputTag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"), FString("InputTag for Right Mouse Button"));
	GameplayTags.InputTag_MMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.MMB"), FString("InputTag for Middle Mouse Button"));
	GameplayTags.InputTag_Space = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Space"), FString("InputTag for Space Bar"));
	GameplayTags.InputTag_0 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.0"), FString("InputTag.0"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"), FString("InputTag.1"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"), FString("InputTag.2"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.3"), FString("InputTag.3"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.4"), FString("InputTag.4"));
	GameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.5"), FString("InputTag.5"));
	GameplayTags.InputTag_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.6"), FString("InputTag.6"));
	GameplayTags.InputTag_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.7"), FString("InputTag.7"));
	GameplayTags.InputTag_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.8"), FString("InputTag.8"));
	GameplayTags.InputTag_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.9"), FString("InputTag.9"));
	GameplayTags.InputTag_E = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.E"), FString("InputTag.E"));
	GameplayTags.InputTag_F = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.F"), FString("InputTag.F"));
	GameplayTags.InputTag_G = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.G"), FString("InputTag.G"));
	GameplayTags.InputTag_D = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.D"), FString("InputTag.D"));
	GameplayTags.InputTag_R = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.R"), FString("InputTag.R"));
	GameplayTags.InputTag_T = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.T"), FString("InputTag.T"));
	GameplayTags.InputTag_C = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.C"), FString("InputTag.C"));
	GameplayTags.InputTag_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Q"), FString("InputTag.Q"));
	GameplayTags.InputTag_LShift = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LShift"), FString("InputTag.LShift"));
	GameplayTags.InputTag_LCtrl = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LCtrl"), FString("InputTag.LCtrl"));
	GameplayTags.InputTag_LAlt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LAlt"), FString("InputTag.LAlt"));
}