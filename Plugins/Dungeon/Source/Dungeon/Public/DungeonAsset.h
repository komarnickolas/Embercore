// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonContainer.h"
#include "SubDungeon.h"
#include "UObject/NoExportTypes.h"
#include "DungeonAsset.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FIterateNodesDelegate, FSubDungeon, SubDungeon, bool, IsLeaf);

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEON_API UDungeonAsset : public UObject {
	GENERATED_BODY()

public:
	UDungeonAsset();

	UFUNCTION(BlueprintCallable)
	void SetStream(FRandomStream InStream);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void Reset();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void GenerateMap();

	UFUNCTION(BlueprintCallable)
	void GenerateMapWithStream(FRandomStream InStream);

	UFUNCTION()
	TArray<FDungeonContainer> SplitDungeonContainer(FDungeonContainer Container);

	UFUNCTION()
	int32 SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex);

	UFUNCTION()
	bool SplitHorizontal(FDungeonContainer Container);

	UFUNCTION()
	float RandomPosition(float In);

	UFUNCTION()
	void GenerateRooms(int32 Index);

	UFUNCTION()
	FDungeonContainer GetRoomFor(int32 Index);

	UFUNCTION()
	void IterateNodes(int32 Index);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIterateNodesDelegate IterateNodesDelegate;

	UFUNCTION()
	FVector GetCenter() const;

	UFUNCTION()
	FVector GetScale() const;

	UFUNCTION()
	FVector GetRandomPointFrom(FDungeonContainer Room);


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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Floor")
	UStaticMesh* FloorMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Floor")
	UMaterial* FloorMaterial;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Floor")
	float FloorWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Floor")
	float FloorHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Wall")
	UStaticMesh* WallMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Room|Wall")
	UMaterial* WallMaterial;

	UPROPERTY(VisibleAnywhere, Category=Map)
	TArray<FSubDungeon> Nodes;
	UPROPERTY()
	FRandomStream CurrentStream;
};
