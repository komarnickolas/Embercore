// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EmbercoreGameMode.generated.h"

UCLASS(minimalapi)
class AEmbercoreGameMode : public AGameModeBase {
	GENERATED_BODY()

public:
	AEmbercoreGameMode();
	void PlayerDied(AController* Controller);

protected:
	float RespawnDelay;

	TSubclassOf<class APlayerCharacter> PlayerClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);
};
