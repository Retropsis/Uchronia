#pragma once

#include "CoreMinimal.h"
#include  "WorldItemData.generated.h"

class AWorldItem_;

USTRUCT(BlueprintType)
struct FWorldItemData
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWorldItem_> ItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ItemName = FName();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ItemDescription = FName();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ItemIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ItemValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsStackable = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackSize = 1;
};