// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbercoreWeapon.h"
#include "UObject/Object.h"
#include "WeaponData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct EMBERCORE_API FWeaponDataStructure {
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FName WeaponId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<class AEmbercoreWeapon> WeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	UTexture2D* WeaponIcon;
};


UCLASS(BlueprintType)
class EMBERCORE_API UWeaponData : public UDataAsset {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Data")
	TArray<FWeaponDataStructure> Data;
};
