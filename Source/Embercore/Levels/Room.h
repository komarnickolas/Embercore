// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UCLASS(Blueprintable)
class EMBERCORE_API ARoom : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoom(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 RoomWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 RoomHeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	float FloorWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	float FloorHeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	UStaticMesh* FloorMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	UMaterial* FloorMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	float WallWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	float WallHeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	UStaticMesh* WallMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	UMaterial* WallMaterial;
	
	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent*> Floor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent*> Wall;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Generate();
	float GetRandomFloat(float Max, float Min);
};
