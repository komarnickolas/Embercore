// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonEditor.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "DungeonAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "DungeonEditor"

void FDungeonEditorModule::StartupModule() {

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	EAssetTypeCategories::Type gameAssetCategory = AssetTools.RegisterAdvancedAssetCategory(
		FName(TEXT("Dungeon")), FText::FromName(TEXT("Dungeon")));

	TSharedPtr<IAssetTypeActions> actionType = MakeShareable(new FDungeonAssetTypeActions(gameAssetCategory));

	AssetTools.RegisterAssetTypeActions(actionType.ToSharedRef());

}

void FDungeonEditorModule::ShutdownModule() {

}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonEditorModule, ExtraAssetsEditor)
