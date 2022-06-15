// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonEditor.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "DungeonBSPTypeActions.h"
#include "DungeonThemeTypeActions.h"

#define LOCTEXT_NAMESPACE "DungeonEditor"

void FDungeonEditorModule::StartupModule() {

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	RegisterAsset(new FDungeonBSPTypeActions(
		AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Dungeon")),
		                                         FText::FromName(TEXT("Dungeon")))));
	RegisterAsset(new FDungeonThemeTypeActions(AssetTools.RegisterAdvancedAssetCategory(
		FName(TEXT("Dungeon")), FText::FromName(TEXT("DungeonTheme")))));
}

void FDungeonEditorModule::RegisterAsset(FAssetTypeActions_Base* AssetTypeActions) {
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<IAssetTypeActions> actionType = MakeShareable(AssetTypeActions);
	AssetTools.RegisterAssetTypeActions(actionType.ToSharedRef());
}

void FDungeonEditorModule::ShutdownModule() {

}
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonEditorModule, ExtraAssetsEditor)
