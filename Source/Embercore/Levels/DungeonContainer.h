#pragma once
#include "DungeonContainer.generated.h"

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
