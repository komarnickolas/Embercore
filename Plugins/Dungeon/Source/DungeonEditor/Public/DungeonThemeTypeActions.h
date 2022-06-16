// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "DungeonTheme.h"


class FDungeonThemeTypeActions : public FAssetTypeActions_Base {
public:
	FDungeonThemeTypeActions(EAssetTypeCategories::Type InAssetCategory);

	virtual FColor GetTypeColor() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<
		                             IToolkitHost>()) override;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return FText::FromName(TEXT("DungeonTheme")); }
	virtual UClass* GetSupportedClass() const override { return UDungeonTheme::StaticClass(); }
	virtual uint32 GetCategories() override { return MyAssetCategory; }

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
