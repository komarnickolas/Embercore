// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "DungeonAsset.h"


class FDungeonAssetTypeActions : public FAssetTypeActions_Base {
public:
	FDungeonAssetTypeActions(EAssetTypeCategories::Type InAssetCategory);

	virtual FColor GetTypeColor() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<
		                             IToolkitHost>()) override;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return FText::FromName(TEXT("DungeonAsset")); }
	virtual UClass* GetSupportedClass() const override { return UDungeonAsset::StaticClass(); }
	virtual uint32 GetCategories() override { return MyAssetCategory; }

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
