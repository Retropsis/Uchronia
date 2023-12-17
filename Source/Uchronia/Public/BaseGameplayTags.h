// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  Singleton containing Native Gameplay Tags
 */
struct FBaseGameplayTags
{
public:
 static const FBaseGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();

 FGameplayTag Attributes_Primary_Strength;
 FGameplayTag Attributes_Primary_Toughness;
 FGameplayTag Attributes_Primary_Dexterity;
 FGameplayTag Attributes_Primary_Perception;
 
 FGameplayTag Attributes_Secondary_Armor;
 FGameplayTag Attributes_Secondary_ArmorPenetration;
 FGameplayTag Attributes_Secondary_BlockChance;
 FGameplayTag Attributes_Secondary_CriticalHitChance;
 FGameplayTag Attributes_Secondary_CriticalHitDamage;
 FGameplayTag Attributes_Secondary_CriticalHitResistance;
 FGameplayTag Attributes_Secondary_HealthRegeneration;
 FGameplayTag Attributes_Secondary_MaxHealth;
 
 FGameplayTag Attributes_Vital_Health;

 FGameplayTag InputTag_LMB;
 FGameplayTag InputTag_RMB;
 FGameplayTag InputTag_MMB;
 FGameplayTag InputTag_Space;
 FGameplayTag InputTag_0;
 FGameplayTag InputTag_1;
 FGameplayTag InputTag_2;
 FGameplayTag InputTag_3;
 FGameplayTag InputTag_4;
 FGameplayTag InputTag_5;
 FGameplayTag InputTag_6;
 FGameplayTag InputTag_7;
 FGameplayTag InputTag_8;
 FGameplayTag InputTag_9;
 FGameplayTag InputTag_E;
 FGameplayTag InputTag_F;
 FGameplayTag InputTag_G;
 FGameplayTag InputTag_D;
 FGameplayTag InputTag_R;
 FGameplayTag InputTag_T;
 FGameplayTag InputTag_C;
 FGameplayTag InputTag_Q;
 FGameplayTag InputTag_LShift;
 FGameplayTag InputTag_LCtrl;
 FGameplayTag InputTag_LAlt;;
 
protected:

private:
 static  FBaseGameplayTags GameplayTags;
};
