// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Weapons/WeaponData.h"
#include "EmbercoreGameMode.generated.h"

UCLASS(minimalapi)
class AEmbercoreGameMode : public AGameMode {
	GENERATED_BODY()

public:
	AEmbercoreGameMode();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerDied();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Weapons")
	FWeaponDataStructure FindWeapon(FName WeaponId, bool& Success);

protected:
	float RespawnDelay;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapons")
	class UWeaponData* WeaponDatabase;
};
