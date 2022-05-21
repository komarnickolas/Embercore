// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Weapons/WeaponData.h"
#include "EmbercoreGameMode.generated.h"

UCLASS(minimalapi)
class AEmbercoreGameMode : public AGameModeBase {
	GENERATED_BODY()

public:
	AEmbercoreGameMode();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerDied();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Weapons")
	FWeaponDataStructure FindWeapon(FName WeaponId, bool& Success);

protected:
	float RespawnDelay;

	TSubclassOf<class AEmbercorePlayer> PlayerClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapons")
	class UWeaponData* WeaponDatabase;
};
