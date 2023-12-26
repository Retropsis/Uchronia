// Retropsis @ 2023-2024

#include "Character/CharacterAnimInstance.h"

#include "Actor/Weapon/Weapon.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TODO: check lazy init
	if(PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if(PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	GroundSpeed = Velocity.Size();

	bAirborne = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = PlayerCharacter->IsWeaponEquipped();
	EquippedWeapon = PlayerCharacter->GetEquippedWeapon();
	bIsCrouched = PlayerCharacter->bIsCrouched;
	bAiming = PlayerCharacter->IsAiming();
	TurningInPlace = PlayerCharacter->GetTurningInPlace();
	bRotateRootBone = PlayerCharacter->ShouldRotateRootBone();

	// Offset Yaw for Strafing
	const FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, YawOffsetInterpSpeed);
	YawOffset = DeltaRotation.Yaw;

	// Leaning
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = PlayerCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, LeanInterpSpeed);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = PlayerCharacter->GetAO_Yaw();
	AO_Pitch = PlayerCharacter->GetAO_Pitch();

	if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && PlayerCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		const FRotator InRotation = LeftHandTransform.GetRotation().Rotator();
		PlayerCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), InRotation, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// TODO: Need HitTarget to replicate (NetQuantize?) for simulated proxies
		if(PlayerCharacter->IsLocallyControlled())
		{
			bIsLocallyControlled = true;
			const FTransform RightHandTransform = PlayerCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			// const FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);

			// RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - PlayerCharacter->GetHitTarget()));
			// RightHandRotation = UKismetMathLibrary::FindLookAtRotation(FVector(), (RightHandTransform.GetLocation() - PlayerCharacter->GetHitTarget()));
			
			// FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FVector(), RightHandTransform.GetLocation() - PlayerCharacter->GetHitTarget());
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - PlayerCharacter->GetHitTarget()));
			LookAtRotation.Roll += EquippedWeapon->RightHandRotationRoll; 
			LookAtRotation.Yaw += EquippedWeapon->RightHandRotationYaw; 
			LookAtRotation.Pitch += EquippedWeapon->RightHandRotationPitch;
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		
			const FTransform MuzzleFlashTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), RTS_World);
			const FVector MuzzleX(FRotationMatrix(MuzzleFlashTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			UKismetSystemLibrary::DrawDebugLine(this, MuzzleFlashTransform.GetLocation(), MuzzleFlashTransform.GetLocation() + MuzzleX * 1000.f, FLinearColor::Blue);
			UKismetSystemLibrary::DrawDebugLine(this, MuzzleFlashTransform.GetLocation(), PlayerCharacter->GetHitTarget(), FLinearColor::Red);
		}
	}
	bUseFABRIK = PlayerCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = PlayerCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = PlayerCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}

void UCharacterAnimInstance::PlayFireMontage(bool bIsAiming)
{
	if(PlayerCharacter->GetCombatComponent() == nullptr || PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	if(IsValid(FireWeaponMontage))
	{
		Montage_Play(FireWeaponMontage);
		const FName SectionName = bIsAiming ? FName("TwoHands_Aim") : FName("TwoHands_Hip");
		Montage_JumpToSection(SectionName);
	}
}

void UCharacterAnimInstance::PlayReloadMontage()
{
	if(PlayerCharacter->GetCombatComponent() == nullptr || PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	if(IsValid(ReloadMontage))
	{
		Montage_Play(ReloadMontage);
		FName SectionName;
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_9mm:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Rocket:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Laser:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_MAX:
			break;
		default: ;
		}
		Montage_JumpToSection(SectionName);
	}
}

void UCharacterAnimInstance::PlayHitReactMontage()
{
	if(PlayerCharacter->GetCombatComponent() == nullptr || PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	
	if(IsValid(HitReactMontage))
	{
		Montage_Play(HitReactMontage);
		const FName SectionName("FromFront");
		Montage_JumpToSection(SectionName);
	}
}
