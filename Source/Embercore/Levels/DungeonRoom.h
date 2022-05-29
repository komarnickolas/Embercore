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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_X)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Y)
	int32 Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_XScale)
	int32 XScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_YScale)
	int32 YScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Width)
	int32 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Height)
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Index)
	int32 Index;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing=OnRep_Cleared)
	bool Cleared = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing=OnRep_Active)
	bool Active = false;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Floor)
	UInstancedStaticMeshComponent* Floor;
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Wall)
	UInstancedStaticMeshComponent* Wall;

	UFUNCTION(BlueprintCallable)
	void Fill();
protected:
	UFUNCTION()
	virtual void OnRep_X();
	UFUNCTION()
	virtual void OnRep_Y();
	UFUNCTION()
	virtual void OnRep_XScale();
	UFUNCTION()
	virtual void OnRep_YScale();
	UFUNCTION()
	virtual void OnRep_Width();
	UFUNCTION()
	virtual void OnRep_Height();
	UFUNCTION()
	virtual void OnRep_Index();
	UFUNCTION()
	virtual void OnRep_Cleared();
	UFUNCTION()
	virtual void OnRep_Active();
	UFUNCTION()
	virtual void OnRep_Floor();
	UFUNCTION()
	virtual void OnRep_Wall();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
