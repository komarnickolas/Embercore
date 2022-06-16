// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonBSPFactory.h"
#include "DungeonBSP.h"
#include "DungeonBSPTypeActions.h"
#include <IAssetTools.h>
#include <Modules/ModuleManager.h>

UDungeonBSPFactory::UDungeonBSPFactory() {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDungeonBSP::StaticClass();
}

UObject* UDungeonBSPFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn) {
	auto MyDungeonAsset = NewObject<UDungeonBSP>(InParent, InClass, InName, Flags);
	return MyDungeonAsset;
}
