// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonThemeTypeActions.h"

FDungeonThemeTypeActions::FDungeonThemeTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory) {
}

FColor FDungeonThemeTypeActions::GetTypeColor() const {
	return FColor::Red;
}

void FDungeonThemeTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                               TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}
