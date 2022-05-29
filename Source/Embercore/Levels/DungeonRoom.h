// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "DungeonRoom.generated.h"

UCLASS(BlueprintType)
class EMBERCORE_API ADungeonRoom : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADungeonRoom();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Index();

	UFUNCTION(BlueprintCallable)
	void SetupRoom(int32 InX, int32 InY, float InXScale, float InYScale, UStaticMesh* FloorMesh,
	               UMaterial* FloorMaterial, UStaticMesh*
	               WallMesh, UMaterial* WallMaterial);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomPoint();
	UFUNCTION(BlueprintCallable)
	FVector GetHalfSize();
	UFUNCTION(BlueprintCallable)
	FVector GetCenter();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 XScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 YScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 Index;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool Cleared = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool Active = false;

	UPROPERTY(EditAnywhere, Replicated)
	ANavMeshBoundsVolume* NavMesh;
	UPROPERTY(EditAnywhere, Replicated)
	UInstancedStaticMeshComponent* Floor;
	UPROPERTY(EditAnywhere, Replicated)
	UInstancedStaticMeshComponent* Wall;

	UFUNCTION(BlueprintCallable)
	void Fill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
