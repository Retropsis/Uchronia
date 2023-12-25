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
#include "TimerManager.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(PlayerCharacter))
	{
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (PlayerCharacter->GetFollowCamera()) 
		{
			DefaultFOV = PlayerCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if(PlayerCharacter->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
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

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(!IsValid(EquippedWeapon)) return;

	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetMarksmanFOV(), DeltaTime, EquippedWeapon->GetMarksmanInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, MarksmanInterpSpeed);		
	}
	if(IsValid(PlayerCharacter) && PlayerCharacter->GetFollowCamera())
	{
		PlayerCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	} 
}

void UCombatComponent::Trigger(const bool bPressed)
{
	bTriggerButtonPressed = bPressed;
	if(bTriggerButtonPressed && IsValid(EquippedWeapon))
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		bCanFire = false;	
		ServerTrigger(HitTarget);
        if(IsValid(EquippedWeapon))
        {
        	// TODO: Weapon Recoil Modifier
        	CrosshairRecoilModifier += .2f;
        }
        FireIntervalStart();
	}
}

bool UCombatComponent::CanFire()
{
	if(EquippedWeapon == nullptr) return false;
	return EquippedWeapon->HasAmmo() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::ServerTrigger_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastTrigger(TraceHitTarget);
}

void UCombatComponent::MulticastTrigger_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon == nullptr) return;
	CharacterAnimInstance = CharacterAnimInstance ? CharacterAnimInstance : Cast<UCharacterAnimInstance>(PlayerCharacter->GetAnimInstance());
	if(CharacterAnimInstance && CombatState == ECombatState::ECS_Unoccupied)
	{
		CharacterAnimInstance->PlayFireMontage(bAiming);
		EquippedWeapon->Trigger(TraceHitTarget);
	}
}

void UCombatComponent::FireIntervalStart()
{
	if(!IsValid(EquippedWeapon) || !IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->GetWorldTimerManager().SetTimer(
		FireIntervalTimer,
		this,
		&UCombatComponent::FireIntervalEnd,
		EquippedWeapon->FireInterval
	);
}

void UCombatComponent::FireIntervalEnd()
{
	if(!IsValid(EquippedWeapon)) return;
	
	bCanFire = true;
	if(bTriggerButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

// TODO: Could have its own WidgetController and Widget
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(!IsValid(PlayerCharacter) || !IsValid(PlayerCharacter->Controller)) return;
	
	CharacterPlayerController = CharacterPlayerController == nullptr ?  Cast<ACharacterPlayerController>(PlayerCharacter->Controller) : CharacterPlayerController;
	if(IsValid(CharacterPlayerController))
	{
		PlayerHUD = PlayerHUD == nullptr ?  Cast<APlayerHUD>(CharacterPlayerController->GetHUD()) : PlayerHUD;
		if(IsValid(PlayerHUD))
		{
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
			if(bAiming)
			{
				CrosshairMarksmanModifier = FMath::FInterpTo(CrosshairMarksmanModifier, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairMarksmanModifier = FMath::FInterpTo(CrosshairMarksmanModifier, 0.f, DeltaTime, 30.f);
			}
			// TODO: Weapon Recoil Recovery
			CrosshairRecoilModifier = FMath::FInterpTo(CrosshairRecoilModifier, 0.f, DeltaTime, 3.f);
			
			// TODO: Try using a min clamp instead
			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityModifier +
				CrosshairAirborneModifier -
				CrosshairMarksmanModifier +
				CrosshairRecoilModifier;
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
		FVector Start = CrosshairWorldPosition;

		if(IsValid(PlayerCharacter))
		{
			const float DistanceToCharacter = (PlayerCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + TraceExtent);
			// UKismetSystemLibrary::DrawDebugSphere(this, Start, 12.f, 12, FLinearColor::Blue);
		}
		
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
		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;			
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
		EquippedWeapon->SetHUDAmmo();
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(PlayerCharacter == nullptr || WeaponToEquip == nullptr) return;
	if(EquippedWeapon)
	{
		EquippedWeapon->Drop();
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* RightHandSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(PlayerCharacter); // is replicated
	EquippedWeapon->SetHUDAmmo();

	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	CharacterPlayerController = CharacterPlayerController == nullptr ?  Cast<ACharacterPlayerController>(PlayerCharacter->Controller) : CharacterPlayerController;
	if(CharacterPlayerController)
	{
		CharacterPlayerController->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}
	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	CharacterPlayerController = CharacterPlayerController == nullptr ?  Cast<ACharacterPlayerController>(PlayerCharacter->Controller) : CharacterPlayerController;
	if(CharacterPlayerController)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Carried Ammo: [%d]"), CarriedAmmo), true, true, FLinearColor::Blue, 3.f);
		CharacterPlayerController->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_9mm, Starting9mmAmmo);
}

void UCombatComponent::Reload()
{
	// if(EquippedWeapon == nullptr) return;
	if(CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::HandleReload()
{
	CharacterAnimInstance = CharacterAnimInstance ? CharacterAnimInstance : Cast<UCharacterAnimInstance>(PlayerCharacter->GetAnimInstance());
	if(CharacterAnimInstance)
	{
		CharacterAnimInstance->PlayReloadMontage();
	}
}

void UCombatComponent::ReloadEnd()
{
	if(PlayerCharacter && PlayerCharacter->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bTriggerButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if(!IsValid(EquippedWeapon)) return;
	
	int32 ReloadAmount = AmountToReload();
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	// Updating Weapon Type Carried Ammo to HUD
	CharacterPlayerController = CharacterPlayerController == nullptr ?  Cast<ACharacterPlayerController>(PlayerCharacter->Controller) : CharacterPlayerController;
	if(CharacterPlayerController)
	{
		CharacterPlayerController->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddRounds(ReloadAmount);
}

int32 UCombatComponent::AmountToReload()
{
	if(EquippedWeapon == nullptr) return 0;
	const int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		const int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		const int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState) {
	case ECombatState::ECS_Unoccupied:
		if(bTriggerButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	default: ;
	}
}
