// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DungeonMap.generated.h"

USTRUCT(BlueprintType)
struct EMBERCORE_API FRectInt {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;

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

	FRectInt(float InX, float InY, float InWidth, float InHeight) {
		X = InX;
		Y = InY;
		Width = InWidth;
		Height = InHeight;
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

	FSubDungeon(): Parent(-1), Index(0), Left(-1), Right(-1) {
	}

	FSubDungeon(FRectInt InContainer): Parent(-1), Index(0), Left(-1), Right(-1) {
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
	void DrawDebugContainer(FRectInt Container, FColor Color, float Z);
	void DrawDebugNode(int32 NodeIndex);
	UFUNCTION(BlueprintCallable)
	void GenerateMap(FRandomStream Stream);
	TArray<FRectInt> SplitDungeonContainer(FRectInt Container);
	int32 SplitDungeon(int32 iteration, FRectInt Container, int32 ParentIndex);
	float RandomPosition(float In);
	void GenerateRooms(int32 Index);
	void GenerateCorridors(int32 Index);
	FVector GetRandomPointFrom(FRectInt Room);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Size;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Depth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinRoomDelta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FSubDungeon> Nodes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRectInt> Corridors;
	UPROPERTY(VisibleAnywhere)
	FRandomStream Stream;
};
