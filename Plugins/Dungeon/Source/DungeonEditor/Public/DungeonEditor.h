// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonAsset.h"
#include "Modules/ModuleManager.h"

class FDungeonEditorModule : public IModuleInterface {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
