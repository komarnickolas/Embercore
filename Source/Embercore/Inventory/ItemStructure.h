#pragma once

#include "ItemStructure.generated.h"

USTRUCT(BlueprintType)
struct EMBERCORE_API FItemStructure {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Description;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AActor> Actor;

	bool operator==(const FItemStructure& OtherStruct) const {
		return Actor == OtherStruct.Actor;
	}

	FItemStructure() {
		Name = "";
		Description = "";
		Actor = nullptr;
	}
};
