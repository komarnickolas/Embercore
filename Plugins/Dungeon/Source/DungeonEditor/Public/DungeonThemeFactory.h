// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DungeonThemeFactory.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONEDITOR_API UDungeonThemeFactory : public UFactory {
	GENERATED_BODY()

public:
	UDungeonThemeFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;


};
