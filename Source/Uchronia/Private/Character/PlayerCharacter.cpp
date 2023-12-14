// Retropsis @ 2023

#include "Character/PlayerCharacter.h"

#include "Actor/Weapon/Weapon.h"
#include "ActorComponents/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/CharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Uchronia/Uchronia.h"

APlayerCharacter::APlayerCharacter()
{
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Display"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 520.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

/*
 * UEngine Functionality
 */
void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->PlayerCharacter = this;
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AimOffset(DeltaSeconds);
	HideCharacterIfCameraClose();
}

void APlayerCharacter::Jump()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

/*
 *  Weapon Functionality
*/
void APlayerCharacter::EquipWeapon()
{
	if(CombatComponent)
	{
		if(HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation()
{
	if(CombatComponent)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}	
}

/*
 * Aiming, AImOffset, TurningInPlace
 */
void APlayerCharacter::Aim(const bool bIsAiming)
{
	if(CombatComponent)
	{
		CombatComponent->SetAiming(bIsAiming);
	}	
}

void APlayerCharacter::TriggerButtonPressed(bool bPressed)
{
	if(CombatComponent)
	{
		CombatComponent->Trigger(bPressed);
	}
}

void APlayerCharacter::HitReact()
{
	MulticastHitReact();
}

void APlayerCharacter::MulticastHitReact_Implementation()
{
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->PlayHitReactMontage();	
}

// TODO: Try having this code in AnimInstance instead
void APlayerCharacter::AimOffset(float DeltaTime)
{
	if(CombatComponent && CombatComponent->EquippedWeapon == nullptr) return; 
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float GroundSpeed = Velocity.Size();
	const bool bAirborne = GetCharacterMovement()->IsFalling();

	if(GroundSpeed == 0.f && !bAirborne) // standing still not jumping
	{
		const FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_None)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(GroundSpeed > 0.f || bAirborne) // running or jumping
	{
		bUseControllerRotationYaw = true;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		TurningInPlace = ETurningInPlace::ETIP_None;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// Mapping pitch from [270, 360) to [-90, 0)
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void APlayerCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace != ETurningInPlace::ETIP_None)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, InterpAO_YawSpeed);
		AO_Yaw = InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_None;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/*
 * End
 */

void APlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(false);
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
	}
}

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) const
{
	if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool APlayerCharacter::IsWeaponEquipped() const
{
	return (CombatComponent && CombatComponent->EquippedWeapon);
}

bool APlayerCharacter::IsAiming() const
{
	return (CombatComponent && CombatComponent->bAiming);
}

AWeapon* APlayerCharacter::GetEquippedWeapon()
{
	if(!IsValid(CombatComponent)) return nullptr;
	return CombatComponent->EquippedWeapon;
}

UAnimInstance* APlayerCharacter::GetAnimInstance() const
{
	if(GetMesh()) return GetMesh()->GetAnimInstance();
	return nullptr;
}

FVector APlayerCharacter::GetHitTarget() const
{
	if(!IsValid(CombatComponent)) return FVector();
	return CombatComponent->HitTarget;
}

void APlayerCharacter::HideCharacterIfCameraClose()
{
	if(!IsLocallyControlled()) return;
	
	const bool bCameraTooClose = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold;
	GEngine->AddOnScreenDebugMessage(1, 0, FColor::Blue, FString::Printf(TEXT("%f"),  (FollowCamera->GetComponentLocation() - GetActorLocation()).Size()));
	GetMesh()->SetVisibility(!bCameraTooClose);
	if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
	{
		CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bCameraTooClose;
	}
}

/*
*
*/
