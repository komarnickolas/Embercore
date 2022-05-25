// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Levels/DungeonMap.h"
#include "EmbercoreGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartRoom);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompleteRoom);

/**
 * 
 */
UCLASS(BlueprintType)
class EMBERCORE_API AEmbercoreGameState : public AGameStateBase {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category=Events)
	FStartRoom OnStartRoom;
	UPROPERTY(BlueprintAssignable, Category=Events)
	FCompleteRoom OnCompleteRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRandomStream Stream;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	int32 CurrentRoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	TSubclassOf<UDungeonMap> MapClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	UDungeonMap* Map;
};
