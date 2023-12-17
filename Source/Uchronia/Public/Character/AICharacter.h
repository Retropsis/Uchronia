// Retropsis @ 2023

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AICharacter.generated.h"

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

protected:
	virtual void BeginPlay() override;

	/*
	 * Ability System
	 */
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;
};
