#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_9mm UMETA(DisplayName="9mm"),
	EWT_Rocket UMETA(DisplayName="Rocket"),

	EWT_MAX  UMETA(DisplayName="DefaultMAX")
};
