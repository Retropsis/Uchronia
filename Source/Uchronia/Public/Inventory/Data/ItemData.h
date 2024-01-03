#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM()
enum class EItemCondition : uint8
{
	EIC_Pristine UMETA(DisplayName="Pristine"),
	EIC_Worn UMETA(DisplayName="Worn"),
	EIC_Damaged UMETA(DisplayName="Damaged"),
	EIC_BadlyDamaged UMETA(DisplayName="BadlyDamaged"),
	EIC_Ruined UMETA(DisplayName="Ruined")
};

UENUM()
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName="Weapon"),
	EIT_Armor UMETA(DisplayName="Armor"),
	EIT_Ammunition UMETA(DisplayName="Ammunition"),
	EIT_Consumable UMETA(DisplayName="Consumable"),
	EIT_Material UMETA(DisplayName="Material"),
	EIT_Misc UMETA(DisplayName="Misc"),
};

USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) FText Name;
	UPROPERTY(EditAnywhere) FText Description;
	UPROPERTY(EditAnywhere) FText Interaction;
	UPROPERTY(EditAnywhere) FText Usage;
};

USTRUCT()
struct FItemStatistics 
{
	GENERATED_BODY()
	
	//~ Damage
	UPROPERTY(EditAnywhere) float FireInterval;
	UPROPERTY(EditAnywhere) float HealthDamage;
	UPROPERTY(EditAnywhere) float HardDamage;
	UPROPERTY(EditAnywhere) float BloodLoss;
	UPROPERTY(EditAnywhere) float DropoffRange;
	UPROPERTY(EditAnywhere) float MinDamageRange;
	UPROPERTY(EditAnywhere) float MaxRange;
	UPROPERTY(EditAnywhere) float AccuracyRadiusAt10m;
	
	//~ Aiming
	UPROPERTY(EditAnywhere) float PerShotModifier;
	UPROPERTY(EditAnywhere) float ShotModifierCap;
	UPROPERTY(EditAnywhere) float RecoveryDelay;
	UPROPERTY(EditAnywhere) float RecoveryPerSecond;
	UPROPERTY(EditAnywhere) float GravityModifier;
	UPROPERTY(EditAnywhere) float CrouchModifier;
	UPROPERTY(EditAnywhere) float WalkModifier;
	UPROPERTY(EditAnywhere) float RunModifier;
	UPROPERTY(EditAnywhere) float SprintModifier;
	UPROPERTY(EditAnywhere) float JumpModifier;
	UPROPERTY(EditAnywhere) float LeanModifier;
	UPROPERTY(EditAnywhere) float MarksmanModifier;
	UPROPERTY(EditAnywhere) float InVehicleModifier;

	//~ Movement

	//~ Ammo
	
	//~ Inventory
	UPROPERTY(EditAnywhere) float Value;
	UPROPERTY(EditAnywhere) float SellValue;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) int32 GridWidth;
	UPROPERTY(EditAnywhere) int32 GridHeight;
	
	UPROPERTY(EditAnywhere) float Weight;
	UPROPERTY(EditAnywhere) int32 MaxStackSize;
	UPROPERTY(EditAnywhere) int32 MaxCharges;
	UPROPERTY(EditAnywhere) bool bIsStackable;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere) TObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere) TObjectPtr<USoundBase> PickupSound;
	
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Item Data") FName ID;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemTextData TextData;
	UPROPERTY(EditAnywhere, Category="Item Data") EItemType ItemType;
	UPROPERTY(EditAnywhere, Category="Item Data") EItemCondition ItemCondition;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemAssetData AssetData;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, Category="Item Data") FItemStatistics ItemStatistics;
};
