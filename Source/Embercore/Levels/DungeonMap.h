// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonAsset.h"
#include "DungeonContainer.h"
#include "DungeonRoom.h"
#include "SubDungeon.h"
#include "DungeonMap.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class EMBERCORE_API ADungeonMap : public AActor {
	GENERATED_BODY()
public:
	ADungeonMap();

	UFUNCTION(BlueprintCallable)
	void ConstructStaticMesh();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION(BlueprintCallable)
	void DrawDebug();

	UFUNCTION()
	void DrawDebugNode(FSubDungeon Node, bool IsLeaf);

	UFUNCTION()
	void DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z);

	UFUNCTION(BlueprintCallable)
	FVector GetCenterOfRect(FDungeonContainer RectInt);

	UFUNCTION(BlueprintCallable)
	FVector SpawnVectorFor(FDungeonContainer Room, float InZ);

	UFUNCTION(BlueprintCallable)
	ADungeonRoom* SpawnRoom(UClass* InClass, FDungeonContainer Room);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDungeonAsset* Dungeon;

	UPROPERTY(VisibleAnywhere)
	FRandomStream Stream;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowDebug;
};
