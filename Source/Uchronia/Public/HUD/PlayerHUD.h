// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UBaseUserWidget;
struct FWidgetControllerParams;

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

	float CrosshairSpread;
	FLinearColor CrosshairColor;
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

	UPROPERTY()
	TObjectPtr<UBaseUserWidget> OverlayWidget;

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);
	
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseUserWidget> OverlayWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	FHUDPackage HUDPackage;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairSpreadMax = 16.f;
	
	void DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& Color);

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
};
