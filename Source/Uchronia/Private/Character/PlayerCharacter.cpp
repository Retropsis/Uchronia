// Retropsis @ 2023

#include "Character/PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "Actor/Item/WorldInteractable.h"
#include "Actor/Weapon/RangeWeapon.h"
#include "Actor/Weapon/Weapon.h"
#include "ActorComponents/CombatComponent.h"
#include "ActorComponents/Inventory/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/CharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/CharacterPlayerController.h"
#include "Player/CharacterPlayerState.h"
#include "Uchronia/Uchronia.h"

APlayerCharacter::APlayerCharacter()
{
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	//~ Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	//~ Camera

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//~ Components
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	// PlayerInventory->SetIsReplicated(true); TODO: Investigate if i need this
	PlayerInventory->SetSlotsCapacity(DefaultInventorySlotsCapacity);
	PlayerInventory->SetWeightCapacity(DefaultInventoryWeightCapacity);
	//~ Components

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 520.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	//~ Overhead, player name
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Display"));
	OverheadWidget->SetupAttachment(GetRootComponent());
}

/*
 * UEngine Functionality
 */
void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(APlayerCharacter, DroppedItem);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(CombatComponent)
	{
		CombatComponent->PlayerCharacter = this;
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//~ Server Init Ability Actor Info
	InitAbilityActorInfo();
	AddCharacterAbilities();
	CombatComponent->UpdateHUDGrenades();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//~ Client Init Ability Actor Info
	InitAbilityActorInfo();
	CombatComponent->UpdateHUDGrenades();
}

void APlayerCharacter::InitAbilityActorInfo()
{
	if(!GetPlayerState<ACharacterPlayerState>()) return; // TODO: Obviously not the correct solution
	ACharacterPlayerState* CharacterPlayerState = GetPlayerState<ACharacterPlayerState>();
	check(CharacterPlayerState)
	CharacterPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(CharacterPlayerState, this);
	Cast<UBaseAbilitySystemComponent>(CharacterPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = CharacterPlayerState->GetAbilitySystemComponent();
	AttributeSet = CharacterPlayerState->GetAttributeSet();

	/* Init Overlay only if PlayerController is not null, only locally controlled needs it */ 
	if (ACharacterPlayerController* CharacterPlayerController = Cast<ACharacterPlayerController>(GetController()))
	{
		// if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(CharacterPlayerController->GetHUD()))
		// {
		// 	PlayerHUD->InitOverlay(CharacterPlayerController, CharacterPlayerState, AbilitySystemComponent, AttributeSet);
		// }
		PlayerHUD = Cast<APlayerHUD>(CharacterPlayerController->GetHUD());
		if(PlayerHUD)
		{
			PlayerHUD->InitOverlay(CharacterPlayerController, CharacterPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaSeconds);
		if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
		{
			PerformInteractionCheck();
		}
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaSeconds;
		if(TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
	HideCharacterIfCameraClose();
	
	// if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	// {
	// 	PerformInteractionCheck();
	// }
}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	// if(GetLocalRole() == ROLE_SimulatedProxy)
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void APlayerCharacter::ToggleMenu()
{
	PlayerHUD->ToggleMenu();
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

void APlayerCharacter::Reload()
{
	if(CombatComponent)
	{
		CombatComponent->Reload();
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
		CombatComponent->Trigger(true);
	}
}

void APlayerCharacter::TriggerButtonReleased(bool bPressed)
{
	if(CombatComponent)
	{
		CombatComponent->Trigger(false);
	}
}

int32 APlayerCharacter::GetCharacterLevel()
{
	const ACharacterPlayerState* CharacterPlayerState = GetPlayerState<ACharacterPlayerState>();
	check(CharacterPlayerState);
	return CharacterPlayerState->GetCharacterLevel();
}

int32 APlayerCharacter::GetGrenadeCount_Implementation()
{
	if(CombatComponent)
	{
		return CombatComponent->GrenadeCount;
	}
	return 0;
}

void APlayerCharacter::IncrementGrenadeCount_Implementation()
{
	if(CombatComponent)
	{
		CombatComponent->GrenadeCount = FMath::Clamp(CombatComponent->GrenadeCount - 1, 0 , CombatComponent->MaxGrenadeCount);
		if(ACharacterPlayerController* CharacterPlayerController = Cast<ACharacterPlayerController>(GetController()))
		{
			CharacterPlayerController->SetHUDGrenadeCount(CombatComponent->GrenadeCount);
		}
	}
}

void APlayerCharacter::HitReact(const FVector& ImpactPoint)
{
	MulticastHitReact(ImpactPoint);
}

void APlayerCharacter::MulticastHitReact_Implementation(const FVector& ImpactPoint)
{
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->PlayHitReactMontage(ImpactPoint);	
}

// TODO: Try having this code in AnimInstance instead
void APlayerCharacter::AimOffset(float DeltaTime)
{
	if(CombatComponent && CombatComponent->EquippedWeapon == nullptr) return;
	const float GroundSpeed = CalculateGroundSpeed();
	const bool bAirborne = GetCharacterMovement()->IsFalling();

	if(GroundSpeed == 0.f && !bAirborne) // standing still not jumping
	{
		bRotateRootBone = true;
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
		bRotateRootBone = false;
		bUseControllerRotationYaw = true;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		TurningInPlace = ETurningInPlace::ETIP_None;
	}
	CalculateAO_Pitch();
}

float APlayerCharacter::CalculateGroundSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void APlayerCharacter::CalculateAO_Pitch()
{
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

void APlayerCharacter::SimProxiesTurn()
{
	if(!IsValid(CombatComponent) || !IsValid(CombatComponent->EquippedWeapon)) return;
	
	bRotateRootBone = false;

	if(CalculateGroundSpeed() > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_None;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if(ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_None;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_None;
}

/*
 * End
 */

//~ Loot Interface ~
void APlayerCharacter::LootAmmunition(const EWeaponType WeaponType, const int32 Amount)
{
	CombatComponent->PickupAmmunition(WeaponType, Amount);
}
//~ Loot Interface End ~


void APlayerCharacter::SetOverlappingWeapon(AWeapon* InWeapon)
{
	if(OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(false);
	OverlappingWeapon = InWeapon;
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
	return (CombatComponent && (CombatComponent->EquippedWeapon || CombatComponent->EquippedMeleeWeapon));
}

bool APlayerCharacter::IsAiming() const
{
	return (CombatComponent && CombatComponent->bAiming);
}

bool APlayerCharacter::IsMelee() const
{
	return  (CombatComponent && CombatComponent->EquippedMeleeWeapon);
}

ARangeWeapon* APlayerCharacter::GetEquippedWeapon()
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

ECombatState APlayerCharacter::GetCombatState() const
{
	if(!IsValid(CombatComponent)) return ECombatState::ECS_MAX;
	return CombatComponent->CombatState;
}

void APlayerCharacter::HideCharacterIfCameraClose()
{
	if(!IsLocallyControlled()) return;
	
	const bool bCameraTooClose = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold;
	GetMesh()->SetVisibility(!bCameraTooClose);
	if (CombatComponent && CombatComponent->EquippedWeapon && CombatComponent->EquippedWeapon->GetWeaponMesh())
	{
		CombatComponent->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bCameraTooClose;
	}
}

void APlayerCharacter::MulticastHandleDeath()
{
	Super::MulticastHandleDeath();
	if(ACharacterPlayerController* CharacterPlayerController = Cast<ACharacterPlayerController>(GetController()))
	{
		CharacterPlayerController->SetHUDWeaponAmmo(0);

		bool bHideAimDownSights = IsLocallyControlled() && CombatComponent && CombatComponent->bAiming &&
			CombatComponent->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_HighCaliberRifle;
		if(bHideAimDownSights)
		{
			CharacterPlayerController->AimDownSights(false);
		}
	}
}

/*
 * Interaction
 */
void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{FollowCamera->GetComponentLocation()};
	
	// TODO: Probably could use the same trace data as combat component traceundercrosshairs
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
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
		TraceStart = CrosshairWorldPosition;
		const float DistanceToCharacter = (GetActorLocation() - TraceStart).Size();
		TraceStart += CrosshairWorldDirection * (DistanceToCharacter + /*TraceExtent*/50.f);
	}
	//
	
	// FVector TraceStart(GetPawnViewLocation());
	FVector TraceEnd(TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance));
	// UKismetSystemLibrary::DrawDebugLine(this, TraceStart, TraceEnd, FLinearColor::Blue, 1.f);

	// DotProduct to check if character looks in same direction as view rotation vector
	float LookDirection(FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector()));
	if(LookDirection > 0)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if(GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if(TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}
				if(TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	NoInteractableFound();
}

void APlayerCharacter::FoundInteractable(AActor* NewInteractable)
{
	if(IsInteracting())
	{
		EndInteract();
	}

	if(InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	PlayerHUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	
	TargetInteractable->BeginFocus();
}

void APlayerCharacter::NoInteractableFound()
{
	if(IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(InteractionCheckTImer);		
	}
	
	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		PlayerHUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void APlayerCharacter::BeginInteract()
{
	// Verify nothing has changed during this interaction state
	PerformInteractionCheck();

	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if(FMath::IsNearlyZero(TargetInteractable->InteractableData.Duration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(InteractionCheckTImer, this, &APlayerCharacter::Interact, TargetInteractable->InteractableData.Duration, false);
			}
		}
	}
}

void APlayerCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(InteractionCheckTImer);
	
	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(InteractionCheckTImer);
	
	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void APlayerCharacter::UpdateInteractionWidget() const
{
	if(IsValid(TargetInteractable.GetObject()))
	{
		PlayerHUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void APlayerCharacter::ServerDropItem_Implementation(const int32 Quantity)
{
	DropItem(DroppedItem, Quantity);
}

void APlayerCharacter::DropItem(UItemBase* ItemToDrop, const int32 Quantity)
{
	if(PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.f) };
		const FTransform SpawnTransform{ GetActorRotation(), SpawnLocation };

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, Quantity);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory didn't find a matching Item"));
	}
}

/*
*
*/
