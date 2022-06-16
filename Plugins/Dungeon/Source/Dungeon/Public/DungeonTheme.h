// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DungeonTheme.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEON_API UDungeonTheme : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	UStaticMesh* FloorMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	UMaterial* FloorMaterial;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	float FloorWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	float FloorHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	UStaticMesh* WallMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	UMaterial* WallMaterial;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	float WallWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	float WallHeight;

	UFUNCTION(BlueprintCallable, Category="Floor")
	void SetMeshAsFloor(UStaticMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category="Wall")
	void SetMeshAsWall(UStaticMeshComponent* MeshComponent);

	void SetMesh(UStaticMeshComponent* MeshComponent, UStaticMesh* Mesh, UMaterial* Material);
};
