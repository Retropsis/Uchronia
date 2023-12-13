// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UTexture2D> Crosshair_Center;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> Crosshair_Left;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> Crosshair_Top;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> Crosshair_Right;
	
	UPROPERTY()
	TObjectPtr<UTexture2D> Crosshair_Bottom;
	
};

/**
 * 
 */
UCLASS()
class UCHRONIA_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
};
