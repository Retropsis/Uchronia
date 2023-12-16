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

protected:
	virtual void BeginPlay() override;

	/*
	 * Ability System
	 */
	virtual void InitAbilityActorInfo() override;
};
