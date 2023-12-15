// Retropsis @ 2023-2024

#include "HUD/PlayerHUD.h"
#include "UI/Widget/BaseUserwidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void APlayerHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class missing, fill out BP_PlayerHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class missing, fill out BP_PlayerHUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UBaseUserWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	
	OverlayWidget->AddToViewport();
}

UOverlayWidgetController* APlayerHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	if(GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		const float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if(IsValid(HUDPackage.Crosshair_Center))
		{
			const FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.Crosshair_Center, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if(IsValid(HUDPackage.Crosshair_Left))
		{
			const FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.Crosshair_Left, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if(IsValid(HUDPackage.Crosshair_Top))
		{
			const FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.Crosshair_Top, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if(IsValid(HUDPackage.Crosshair_Right))
		{
			const FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.Crosshair_Right, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
		if(IsValid(HUDPackage.Crosshair_Bottom))
		{
			const FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.Crosshair_Bottom, ViewportCenter, Spread, HUDPackage.CrosshairColor);
		}
	}
}

void APlayerHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& Color)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X -  (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y,
		TextureWidth, TextureHeight,
		0.f, 0.f, 1.f, 1.f,
		Color
	);
}
