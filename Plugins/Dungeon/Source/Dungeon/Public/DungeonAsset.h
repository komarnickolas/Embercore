// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonContainer.h"
#include "SubDungeon.h"
#include "UObject/NoExportTypes.h"
#include "DungeonAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEON_API UDungeonAsset : public UObject {
	GENERATED_BODY()

public:
	UDungeonAsset();
	void SetStream(FRandomStream InStream);
	void GenerateMap();
	void GenerateMap(FRandomStream InStream);
	TArray<FDungeonContainer> SplitDungeonContainer(FDungeonContainer Container);
	int32 SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex);
	bool SplitHorizontal(FDungeonContainer Container);
	float RandomPosition(float In);
	void GenerateRooms(int32 Index);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	int32 Size;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	int32 Depth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Room)
	int32 MinRoomDelta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Room)
	int32 MinRoomWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Room)
	int32 MinRoomHeight;

	UPROPERTY(VisibleAnywhere, Category=Map)
	TArray<FSubDungeon> Nodes;
	UPROPERTY()
	FRandomStream CurrentStream;
};
