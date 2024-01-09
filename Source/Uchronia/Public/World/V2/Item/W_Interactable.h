// Retropsis @ 2023-2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractionInterface.h"
#include "W_Interactable.generated.h"

UCLASS()
class UCHRONIA_API AW_Interactable : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AW_Interactable();
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(APlayerCharacter* PlayerCharacter) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	FText ItemName = FText::GetEmpty();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interactable Properties")
	TObjectPtr<UStaticMeshComponent> InteractableMesh;

protected:
	virtual void BeginPlay() override;
};
