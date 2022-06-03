// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonAssetTypeActions.h"

FDungeonAssetTypeActions::FDungeonAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory) {
}

FColor FDungeonAssetTypeActions::GetTypeColor() const {
	return FColor::Red;
}

void FDungeonAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                               TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}
