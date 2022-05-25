// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "DungeonRoom.generated.h"

UCLASS(BlueprintType)
class EMBERCORE_API ADungeonRoom : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonRoom();

	UFUNCTION(BlueprintCallable)
	void BuildRoom(int32 InX, int32 InY, float XScale, float YScale, UStaticMesh* FloorMesh, UMaterial* FloorMaterial,
	               UStaticMesh*
	               WallMesh, UMaterial* WallMaterial);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool Cleared = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool Active = false;

	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* Floor;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* Wall;
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
