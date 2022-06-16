// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonBSP.h"
#include "GameFramework/Actor.h"
#include "DungeonMesh.generated.h"

UCLASS(BlueprintType)
class DUNGEON_API ADungeonMesh : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDungeonBSP* Dungeon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInstancedStaticMeshComponent* Floor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInstancedStaticMeshComponent* Wall;

	UFUNCTION()
	void DrawMap(FSubDungeon SubDungeon, bool bIsLeaf);

	UFUNCTION()
	void AddFloor(FDungeonContainer Room);

	UFUNCTION()
	void AddWalls(FDungeonContainer Room);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;


};
