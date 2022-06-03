// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonAssetFactory.h"
#include "DungeonAsset.h"
#include "DungeonAssetTypeActions.h"
#include <IAssetTools.h>
#include <Modules/ModuleManager.h>

UDungeonAssetFactory::UDungeonAssetFactory() {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDungeonAsset::StaticClass();
}

UObject* UDungeonAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn) {
	auto MyDungeonAsset = NewObject<UDungeonAsset>(InParent, InClass, InName, Flags);
	return MyDungeonAsset;
}
