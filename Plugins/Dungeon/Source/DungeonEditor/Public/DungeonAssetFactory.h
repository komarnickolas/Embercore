// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DungeonAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONEDITOR_API UDungeonAssetFactory : public UFactory {
	GENERATED_BODY()

public:
	UDungeonAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;


};
