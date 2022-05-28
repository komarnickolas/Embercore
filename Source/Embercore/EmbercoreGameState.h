// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Levels/DungeonMap.h"
#include "EmbercoreGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartRoom, ADungeonRoom*, Room);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompleteRoom, ADungeonRoom*, Room);

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentRoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ADungeonRoom* CurrentRoomActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ADungeonMap* CurrentMap;
};
