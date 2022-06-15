// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "DungeonBSP.h"
#include "IAssetTools.h"
#include "Modules/ModuleManager.h"

class FDungeonEditorModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	void RegisterAsset(FAssetTypeActions_Base* AssetTypeActions);
	virtual void ShutdownModule() override;
};
