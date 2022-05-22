// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Levels/DungeonMap.h"
#include "EmbercoreGameState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class EMBERCORE_API AEmbercoreGameState : public AGameStateBase {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRandomStream Stream;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UDungeonMap> MapClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDungeonMap* Map;
};
