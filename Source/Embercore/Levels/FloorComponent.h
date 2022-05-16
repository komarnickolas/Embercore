// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Math/TransformCalculus3D.h"
#include "FloorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EMBERCORE_API UFloorComponent : public USceneComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFloorComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dimensions")
	int32 Width;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Dimensions")
	int32 Length;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh")
	float FloorWidth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh")
	float FloorLength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh")
	UStaticMesh* FloorMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh")
	UMaterial* FloorMaterial;

	UFUNCTION(BlueprintCallable)
	void Generate();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UStaticMeshComponent* CreateFloor(FString Name);
	void AddFloor(float x, float y);


};
