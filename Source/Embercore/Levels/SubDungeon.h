#pragma once
#include "DungeonContainer.h"
#include "SubDungeon.generated.h"

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
