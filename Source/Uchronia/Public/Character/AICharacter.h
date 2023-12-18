// Retropsis @ 2023

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AICharacter.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class UCHRONIA_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAICharacter();
	
	/* Combat Interface */
	virtual int32 GetCharacterLevel() override;
	virtual void HitReact() override;
	/* Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;

	/*
	 * Ability System
	 */
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
