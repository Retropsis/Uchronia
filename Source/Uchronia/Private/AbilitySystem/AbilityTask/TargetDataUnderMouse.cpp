// Retropsis @ 2023-2024

#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	// TODO: Can pass more infos into MyObj
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	//APlayerCharacter* PlayerCharacter = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	// PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FCollisionQueryParams CollisionQueryParams;
	PlayerController->GetHitResultAtScreenPosition(ViewportCenter, ECC_Visibility, CollisionQueryParams, CursorHit);
	
	ValidData.Broadcast(CursorHit.Location);
}
