// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonThemeFactory.h"
#include "DungeonTheme.h"
#include "DungeonThemeTypeActions.h"
#include <IAssetTools.h>
#include <Modules/ModuleManager.h>

UDungeonThemeFactory::UDungeonThemeFactory() {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDungeonTheme::StaticClass();
}

UObject* UDungeonThemeFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn) {
	return NewObject<UDungeonTheme>(InParent, InClass, InName, Flags);
}
