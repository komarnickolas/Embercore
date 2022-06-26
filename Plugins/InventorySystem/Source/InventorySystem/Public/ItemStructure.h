#pragma once

#include "ItemStructure.generated.h"

USTRUCT(BlueprintType)
struct INVENTORYSYSTEM_API FItemStructure {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Description;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UObject> Object;

	bool operator==(const FItemStructure& OtherStruct) const {
		return Object == OtherStruct.Object;
	}

	FItemStructure() {
		Name = "";
		Description = "";
		Object = nullptr;
	}
};
