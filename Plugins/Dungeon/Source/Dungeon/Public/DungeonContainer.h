#pragma once
#include "DungeonContainer.generated.h"

USTRUCT(BlueprintType)
struct DUNGEON_API FDungeonContainer {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Min;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Max;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D XMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D YMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Center;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;

	FDungeonContainer(): Min(FVector2D()), Max(FVector2D()), XMax(FVector2D()), YMax(FVector2D()), Width(0), Height(0) {
	}

	FDungeonContainer(int32 InX, int32 InY, int32 InWidth, int32 InHeight) {
		Width = InWidth;
		Height = InHeight;
		Min = FVector2D(InX, InY);
		Max = FVector2D(InX + Width, InY + Height);
		XMax = FVector2D(Max.X, Min.Y);
		YMax = FVector2D(Min.X, Max.Y);
		Center = Max / 2;
	}
};
