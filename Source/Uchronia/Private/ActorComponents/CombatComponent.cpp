// Retropsis @ 2023-2024

#include "ActorComponents/CombatComponent.h"
#include "Actor/Weapon/Weapon.h"
#include "Character/CharacterAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/CharacterPlayerController.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHUDCrosshairs(DeltaTime);
}

/*
 * Combat Functionality
 */
void UCombatComponent::SetAiming(const bool IsAiming)
{
	bAiming = IsAiming;
	ServerSetAiming(IsAiming);
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::Trigger(const bool bPressed)
{
	bTriggerButtonPressed = bPressed;
	if(bTriggerButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		ServerTrigger(HitResult.ImpactPoint);
	}
}

void UCombatComponent::ServerTrigger_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastTrigger(TraceHitTarget);
}

void UCombatComponent::MulticastTrigger_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	CharacterAnimInstance = CharacterAnimInstance ? CharacterAnimInstance : Cast<UCharacterAnimInstance>(PlayerCharacter->GetAnimInstance());
	if(EquippedWeapon == nullptr) return;
	if(IsValid(PlayerCharacter) && CharacterAnimInstance)
	{
		CharacterAnimInstance->PlayFireMontage(bAiming);
		EquippedWeapon->Trigger(TraceHitTarget);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(!IsValid(PlayerCharacter) || !IsValid(PlayerCharacter->Controller)) return;
	
	CharacterPlayerController = CharacterPlayerController == nullptr ?  Cast<ACharacterPlayerController>(PlayerCharacter->Controller) : CharacterPlayerController;
	if(IsValid(CharacterPlayerController))
	{
		PlayerHUD = PlayerHUD == nullptr ?  Cast<APlayerHUD>(CharacterPlayerController->GetHUD()) : PlayerHUD;
		if(IsValid(PlayerHUD))
		{
			FHUDPackage HUDPackage;
			if(EquippedWeapon)
			{
				HUDPackage.Crosshair_Center = EquippedWeapon->Crosshair_Center;
				HUDPackage.Crosshair_Left = EquippedWeapon->Crosshair_Left;
				HUDPackage.Crosshair_Top = EquippedWeapon->Crosshair_Top;
				HUDPackage.Crosshair_Right = EquippedWeapon->Crosshair_Right;
				HUDPackage.Crosshair_Bottom = EquippedWeapon->Crosshair_Bottom;
			}
			else
			{
				// TODO: Set to Unequipped Crosshairs
				HUDPackage.Crosshair_Center = nullptr;
				HUDPackage.Crosshair_Left = nullptr;
				HUDPackage.Crosshair_Top = nullptr;
				HUDPackage.Crosshair_Right = nullptr;
				HUDPackage.Crosshair_Bottom = nullptr;
			}
			// Calculate Crosshair Spread
			// [0, 600] -> [0, 1]
			// TODO: APB-like spread
			const FVector2D WalkSpeedRange(0.f, PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed);
			const FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = PlayerCharacter->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityModifier = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if(PlayerCharacter->GetCharacterMovement()->IsFalling())
			{
				// TODO: Weapon Airborne Modifier
				CrosshairAirborneModifier = FMath::FInterpTo(CrosshairAirborneModifier, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				// TODO: Weapon Airborne Recovery Modifier
				CrosshairAirborneModifier = FMath::FInterpTo(CrosshairAirborneModifier, 0.f, DeltaTime, 30.f);
			}
			HUDPackage.CrosshairSpread = CrosshairVelocityModifier + CrosshairAirborneModifier;
			PlayerHUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	// TODO: Could add a parameterizable offset x and y
	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if(bScreenToWorld)
	{
		const FVector Start = CrosshairWorldPosition;
		const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		if(!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

void UCombatComponent::ServerSetAiming_Implementation(const bool IsAiming)
{
	bAiming = IsAiming;
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if(IsValid(EquippedWeapon) && IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(PlayerCharacter == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* RightHandSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(PlayerCharacter); // is replicated

	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->bUseControllerRotationYaw = true;
}

