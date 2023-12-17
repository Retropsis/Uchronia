// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UCHRONIA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetCharacterLevel();
	virtual void HitReact() = 0;
};
