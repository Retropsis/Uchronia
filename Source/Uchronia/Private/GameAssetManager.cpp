// Retropsis @ 2023-2024


#include "GameAssetManager.h"
#include "BaseGameplayTags.h"

UGameAssetManager& UGameAssetManager::Get()
{
	check(GEngine);
	UGameAssetManager* GameAssetManager = Cast<UGameAssetManager>(GEngine->AssetManager);
	return *GameAssetManager;
}

void UGameAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FBaseGameplayTags::InitializeNativeGameplayTags();
}
