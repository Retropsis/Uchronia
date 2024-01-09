// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "World/V2/Item/W_Interactable.h"
#include "W_Item.generated.h"

/**
 * 
 */
UCLASS()
class UCHRONIA_API AW_Item : public AW_Interactable
{
	GENERATED_BODY()

public:
	virtual void Interact(APlayerCharacter* PlayerCharacter) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	FText ItemDescription = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	TObjectPtr<UTexture2D> ItemIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	TObjectPtr<USoundBase> PickupSound;
};
