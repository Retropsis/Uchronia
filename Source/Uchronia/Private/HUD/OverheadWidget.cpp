// Retropsis @ 2023-2024


#include "HUD/OverheadWidget.h"

#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(const FString& TextToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	const ENetRole LocalRole = InPawn->GetLocalRole();
	const ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ROLE_None:
		Role = FString("None");
		break;
	default: ;
	}
	FString RemoteRoleString = FString::Printf(TEXT("RemoteRole: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (APlayerState* PlayerState = InPawn->GetPlayerState())
	{
		FString PlayerName = PlayerState->GetPlayerName();
		SetDisplayText(PlayerName);
	}
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
