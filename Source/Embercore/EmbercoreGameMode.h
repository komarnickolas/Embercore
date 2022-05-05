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
	void PlayerDied(AController* Controller);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Weapons")
	FWeaponDataStructure FindWeapon(FName WeaponId, bool& Success);

protected:
	float RespawnDelay;

	TSubclassOf<class APlayerCharacter> PlayerClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapons")
	class UWeaponData* WeaponDatabase;
};
