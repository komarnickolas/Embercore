// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonBSPTypeActions.h"

FDungeonBSPTypeActions::FDungeonBSPTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory) {
}

FColor FDungeonBSPTypeActions::GetTypeColor() const {
	return FColor::Red;
}

void FDungeonBSPTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                               TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}
