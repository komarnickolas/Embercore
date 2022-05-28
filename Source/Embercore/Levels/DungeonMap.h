// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonRoom.h"
#include "DungeonMap.generated.h"

USTRUCT(BlueprintType)
struct EMBERCORE_API FDungeonContainer {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 YMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool Cleared = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool Active = false;

	float GetXCenter() const {
		return (X + GetXMax()) / 2;
	}

	float GetYCenter() const {
		return (Y + GetYMax()) / 2;
	}

	float GetYMax() const {
		return Y + Height;
	}

	float GetXMax() const {
		return X + Width;
	}

	FVector GetCenter(float Z = 0) const {
		return FVector(GetXCenter(), GetYCenter(), Z);
	}

	FVector ToVector(float Z = 0) const {
		return FVector(X, Y, Z);
	}

	FVector ToMaxVector(float Z = 0) const {
		return FVector(GetXMax(), GetYMax(), Z);
	}

	FVector ToXMaxVector(float Z = 0) const {
		return FVector(GetXMax(), Y, Z);
	}

	FVector ToYMaxVector(float Z = 0) const {
		return FVector(X, GetYMax(), Z);
	}

	void Print() {
		UE_LOG(LogTemp, Warning,
		       TEXT("X: %i Y:%i Width: %i Height: %i Center:(%i,%i)"),
		       X, Y, Width, Height, GetXCenter(), GetYCenter());
	}

	FDungeonContainer(): X(0), XMax(0), Y(0), YMax(0), Width(0), Height(0) {
	}

	FDungeonContainer(int32 InX, int32 InY, int32 InWidth, int32 InHeight) {
		X = InX;
		Y = InY;
		Width = InWidth;
		Height = InHeight;
		XMax = X + Width;
		YMax = Y + Height;
	}
};

USTRUCT(BlueprintType)
struct EMBERCORE_API FSubDungeon {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDungeonContainer Container;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDungeonContainer Room;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Parent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Depth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Right;

	bool IsLeaf() const {
		return Left == -1 && Right == -1;
	}

	bool IsInternal() const {
		return Left != -1 || Right != -1;
	}

	FSubDungeon(): Parent(-1), Index(0), Depth(0), Left(-1), Right(-1) {
	}

	FSubDungeon(FDungeonContainer InContainer): Parent(-1), Index(0), Depth(0), Left(-1), Right(-1) {
		this->Container = InContainer;
	}
};

/**
 * 
 */
UCLASS(BlueprintType)
class EMBERCORE_API ADungeonMap : public AActor {
	GENERATED_BODY()
public:
	ADungeonMap();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION(BlueprintCallable)
	void DrawDebug();

	UFUNCTION(BlueprintCallable)
	FVector GetCenterOfRect(FDungeonContainer RectInt);

	UFUNCTION(BlueprintCallable)
	FDungeonContainer GetStartingRoom();

	UFUNCTION(BlueprintCallable)
	FDungeonContainer GetEndingRoom();

	void DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z);
	void DrawDebugNode(int32 NodeIndex);

	UFUNCTION(BlueprintCallable)
	void GenerateMap(FRandomStream InStream);

	bool SplitHorizontal(FDungeonContainer Container);
	TArray<FDungeonContainer> SplitDungeonContainer(FDungeonContainer Container);
	int32 SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex);

	UFUNCTION(BlueprintCallable)
	float RandomPosition(float In);

	void GenerateRooms(int32 Index);

	UFUNCTION(BlueprintCallable)
	FDungeonContainer GetRoomFor(int32 Index);

	UFUNCTION(BlueprintCallable)
	FVector SpawnVectorFor(FDungeonContainer Room, float InZ);

	UFUNCTION(BlueprintCallable)
	ADungeonRoom* SpawnRoom(UClass* InClass, FDungeonContainer Room);

	UFUNCTION(BlueprintCallable)
	void SpawnNode(UClass* InClass, int32 Index);

	UFUNCTION(BlueprintCallable)
	void SpawnMap(UClass* InClass);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomPointFrom(FDungeonContainer Room);

	UFUNCTION(BlueprintCallable)
	bool IsLeaf(FSubDungeon SubDungeon);

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Map)
	TArray<FSubDungeon> Nodes;
	UPROPERTY(VisibleAnywhere)
	FRandomStream Stream;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowDebug;

	UFUNCTION(BlueprintCallable)
	FVector GetMapCenter();

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FIterateNodes, FSubDungeon, SubDungeon, bool, IsLeaf);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FIterateLeafs, FSubDungeon, SubDungeon);

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FIterateRect, float, X, float, Y);

	UFUNCTION(BlueprintCallable)
	void IterateNodes(FIterateNodes Functor, int32 Index);
	UFUNCTION(BlueprintCallable)
	void IterateLeafs(FIterateLeafs Functor, int32 Index);
	UFUNCTION(BlueprintCallable)
	void IterateRoom(FIterateRect Iterator, FIterateRect XIterator, FIterateRect YIterator, FDungeonContainer Rect);
	UFUNCTION(BlueprintCallable)
	void IterateEntireRoom(FIterateRect Iterator, FDungeonContainer Rect);
	UFUNCTION(BlueprintCallable)
	void IterateRoomX(FIterateRect Iterator, FDungeonContainer Rect);
	UFUNCTION(BlueprintCallable)
	void IterateRoomY(FIterateRect Iterator, FDungeonContainer Rect);

};
