#pragma once

#define TRACE_LENGTH 80000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_9mm UMETA(DisplayName="9mm"),
	EWT_Laser UMETA(DisplayName="Laser"),
	EWT_Rocket UMETA(DisplayName="Rocket"),
	EWT_SubmachineGun UMETA(DisplayName="SubmachineGun"),
	EWT_Shotgun UMETA(DisplayName="Shotgun"),

	EWT_MAX  UMETA(DisplayName="DefaultMAX")
};
