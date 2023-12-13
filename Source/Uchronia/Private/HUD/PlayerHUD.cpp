// Retropsis @ 2023-2024


#include "HUD/PlayerHUD.h"

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if(IsValid(HUDPackage.Crosshair_Center)) DrawCrosshair(HUDPackage.Crosshair_Center, ViewportCenter);
		if(IsValid(HUDPackage.Crosshair_Left)) DrawCrosshair(HUDPackage.Crosshair_Left, ViewportCenter);
		if(IsValid(HUDPackage.Crosshair_Top)) DrawCrosshair(HUDPackage.Crosshair_Top, ViewportCenter);
		if(IsValid(HUDPackage.Crosshair_Right)) DrawCrosshair(HUDPackage.Crosshair_Right, ViewportCenter);
		if(IsValid(HUDPackage.Crosshair_Bottom)) DrawCrosshair(HUDPackage.Crosshair_Bottom, ViewportCenter);
	}
}

void APlayerHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X -  (TextureWidth / 2.f),
		ViewportCenter.Y - (TextureHeight / 2.f)
	);
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y,
		TextureWidth, TextureHeight,
		0.f, 0.f, 1.f, 1.f,
		FLinearColor::White
	);
}
