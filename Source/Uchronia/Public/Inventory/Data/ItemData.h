#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemCondition : uint8
{
	EIC_Pristine UMETA(DisplayName="Pristine"),
	EIC_Worn UMETA(DisplayName="Worn"),
	EIC_Damaged UMETA(DisplayName="Damaged"),
	EIC_BadlyDamaged UMETA(DisplayName="BadlyDamaged"),
	EIC_Ruined UMETA(DisplayName="Ruined")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName="Weapon"),
	EIT_Armor UMETA(DisplayName="Armor"),
	EIT_Ammunition UMETA(DisplayName="Ammunition"),
	EIT_Consumable UMETA(DisplayName="Consumable"),
	EIT_Material UMETA(DisplayName="Material"),
	EIT_Misc UMETA(DisplayName="Misc"),
};

USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Interaction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Usage;
};

USTRUCT(BlueprintType)
struct FItemStatistics 
{
	GENERATED_BODY()
	
	//~ Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FireInterval;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HealthDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HardDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float BloodLoss;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DropoffRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MinDamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float AccuracyRadiusAt10m;
	
	//~ Aiming
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float PerShotModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ShotModifierCap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float RecoveryDelay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float RecoveryPerSecond;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float GravityModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CrouchModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float WalkModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float RunModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SprintModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float JumpModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float LeanModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MarksmanModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float InVehicleModifier;

	//~ Movement

	//~ Ammo
	
	//~ Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Value;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SellValue;
};

USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 GridWidth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 GridHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxStackSize;
	UPROPERTY(BlueprintReadOnly) bool bIsStackable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 MaxCharges;
	UPROPERTY(BlueprintReadOnly) bool bHasCharges;
};

USTRUCT(BlueprintType, Blueprintable)
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UStaticMesh> Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<USoundBase> PickupSound;
};

USTRUCT(BlueprintType, Blueprintable)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") FName ID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") FItemTextData TextData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") EItemCondition ItemCondition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") FItemAssetData AssetData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data") FItemStatistics ItemStatistics;
};
