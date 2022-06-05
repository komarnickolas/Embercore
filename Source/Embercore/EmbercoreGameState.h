// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Levels/DungeonMap.h"
#include "EmbercoreGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartRoom, ADungeonRoom*, Room);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompleteRoom, ADungeonRoom*, Room);

/**
 * 
 */
UCLASS(BlueprintType)
class EMBERCORE_API AEmbercoreGameState : public AGameState {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category=Events)
	FStartRoom OnStartRoom;
	UPROPERTY(BlueprintAssignable, Category=Events)
	FCompleteRoom OnCompleteRoom;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	FRandomStream Stream;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int32 CurrentRoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	ADungeonRoom* CurrentRoomActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ADungeonMap* CurrentMap;
};
