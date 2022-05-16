// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DungeonMap.generated.h"
USTRUCT(BlueprintType)
struct EMBERCORE_API FDungeonTree {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FVector4 Container;
	UPROPERTY(BlueprintReadOnly)
	FVector4 Room;
	UPROPERTY(BlueprintReadOnly)
	int32 Index;
	UPROPERTY(BlueprintReadOnly)
	int32 Left;
	UPROPERTY(BlueprintReadOnly)
	int32 Right;

	bool IsLeaf() const {
		return Left == -1 && Right == -1;
	}

	bool IsInternal() const {
		return Left != -1 || Right != -1;
	}

	FDungeonTree(): Index(0), Left(-1), Right(-1) {
	}

	FDungeonTree(FVector4 C): Index(0), Left(-1), Right(-1) {
		this->Container = C;
	}
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class EMBERCORE_API UDungeonMap : public UObject {
	GENERATED_BODY()
public:
	UDungeonMap();
	UFUNCTION(BlueprintCallable)
	void DrawDebug();
	void DrawDebugContainer(FVector4 Container, FColor Color, float Z);
	void DrawDebugNode(int32 NodeIndex);
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FRandomStream Stream);
	TArray<FVector4> SplitDungeonContainer(FVector4 Container);
	int32 SplitDungeon(int32 iteration, FVector4 Container);
	float RandomPosition(float In);
	void GenerateRooms(int32 Index);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Size;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Depth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinRoomDelta;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FDungeonTree> Nodes;
	UPROPERTY(VisibleAnywhere)
	FRandomStream Stream;
};
