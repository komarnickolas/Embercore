// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonContainer.h"
#include "DungeonTheme.h"
#include "SubDungeon.h"
#include "UObject/NoExportTypes.h"
#include "DungeonBSP.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FIterateNodesDelegate, FSubDungeon, SubDungeon, bool, IsLeaf);

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEON_API UDungeonBSP : public UObject {
	GENERATED_BODY()

public:
	UDungeonBSP();

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

	UFUNCTION(BlueprintCallable)
	void IterateNodes(FIterateNodesDelegate IterateNodesDelegate, int32 Index);

	UFUNCTION()
	FVector GetCenter() const;

	UFUNCTION(BlueprintCallable)
	FVector GetScale() const;

	UFUNCTION(BlueprintCallable)
	void SetContext(UObject* InContext);

	UFUNCTION(BlueprintCallable)
	void DrawDebug();

	UFUNCTION()
	void DrawDebugNode(FSubDungeon Node, bool IsLeaf);

	UFUNCTION()
	void DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z);
	float ScaleX(float InX);
	float ScaleY(float InY);

	UFUNCTION(BlueprintCallable)
	FVector Scale(FVector2D Vector, float InZ = 0);

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Theme")
	UDungeonTheme* Theme;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShowDebug;

	UPROPERTY(VisibleAnywhere, Category=Map)
	TArray<FSubDungeon> Nodes;
	UPROPERTY()
	FRandomStream CurrentStream;

	UObject* Context;
};
