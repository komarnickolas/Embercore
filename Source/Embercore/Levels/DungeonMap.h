// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DungeonMap.generated.h"

USTRUCT(BlueprintType)
struct EMBERCORE_API FRectInt {
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

	FRectInt(): X(0), Y(0), Width(0), Height(0) {
	}

	FRectInt(int32 InX, int32 InY, int32 InWidth, int32 InHeight) {
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
	FRectInt Container;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRectInt Room;
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

	void Print() {
		UE_LOG(LogTemp, Warning, TEXT("Index: %i Parent: %i, Left: %i Right: %i"), Index, Parent, Left, Right);
		UE_LOG(LogTemp, Warning, TEXT("Container: "));
		Container.Print();
		UE_LOG(LogTemp, Warning, TEXT("Room: "));
		Room.Print();
	}

	FSubDungeon(): Parent(-1), Index(0), Depth(0), Left(-1), Right(-1) {
	}

	FSubDungeon(FRectInt InContainer): Parent(-1), Index(0), Depth(0), Left(-1), Right(-1) {
		this->Container = InContainer;
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
	UFUNCTION(BlueprintCallable)
	FRectInt GetStartingRoom();
	UFUNCTION(BlueprintCallable)
	FRectInt GetEndingRoom();
	void DrawDebugContainer(FRectInt Container, FColor Color, float Z);
	void DrawDebugNode(int32 NodeIndex);
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FRandomStream InStream);
	bool SplitHorizontal(FRectInt Container);
	TArray<FRectInt> SplitDungeonContainer(FRectInt Container);
	int32 SplitDungeon(int32 iteration, FRectInt Container, int32 ParentIndex);
	float RandomPosition(float In);
	void GenerateRooms(int32 Index);
	FRectInt GetRoomFor(int32 Index);
	void GenerateCorridorBetween(int32 LeftIndex, int32 RightIndex);
	FVector GetRandomPointFrom(FRectInt Room);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Size;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Depth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinRoomDelta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinRoomWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinRoomHeight;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 Scale;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UStaticMesh* FloorMesh;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UStaticMesh* WallMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSubDungeon> Nodes;
	UPROPERTY(VisibleAnywhere)
	FRandomStream Stream;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRectInt> Corridors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowDebug;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FIterateNodes, FSubDungeon, SubDungeon);

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FIterateRect, float, X, float, Y);

	UFUNCTION(BlueprintCallable)
	void IterateNodes(FIterateNodes Functor, int32 Index);
	UFUNCTION(BlueprintCallable)
	void IterateRoom(FIterateRect Iterator, FIterateRect XIterator, FIterateRect YIterator, FRectInt Rect);
	UFUNCTION(BlueprintCallable)
	void IterateEntireRoom(FIterateRect Iterator, FRectInt Rect);
	UFUNCTION(BlueprintCallable)
	void IterateRoomX(FIterateRect Iterator, FRectInt Rect);
	UFUNCTION(BlueprintCallable)
	void IterateRoomY(FIterateRect Iterator, FRectInt Rect);
};
