// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom.h"

#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ADungeonRoom::ADungeonRoom() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Room")));
	Floor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Floor"));
	Floor->SetupAttachment(RootComponent);
	Wall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Wall"));
	Wall->SetupAttachment(RootComponent);
}

void ADungeonRoom::BuildRoom(int32 InX, int32 InY, float InXScale, float InYScale, UStaticMesh* FloorMesh,
                             UMaterial* FloorMaterial,
                             UStaticMesh* WallMesh, UMaterial* WallMaterial) {
	X = InX;
	Y = InY;
	XScale = InXScale;
	YScale = InYScale;
	Width = X * XScale;
	Height = Y * YScale;
	Floor->SetStaticMesh(FloorMesh);
	Floor->SetMaterial(0, FloorMaterial);
	Wall->SetStaticMesh(WallMesh);
	Wall->SetMaterial(0, WallMaterial);
	for (int x = 0; x < X; x++) {
		Wall->AddInstance(FTransform(FVector(x * XScale, 0, 0)));
		Wall->AddInstance(FTransform(FVector(x * XScale, Y * YScale, 0)));
		for (int y = 0; y < Y; y++) {
			Floor->AddInstance(FTransform(FVector(x * XScale, y * YScale, 0)));
		}
	}
	for (int y = 0; y < Y; y++) {
		FTransform TransformTop = FTransform(
			FRotator(0, 90, 0),
			FVector(0, y * YScale, 0)
		);
		FTransform TransformBottom = FTransform(
			FRotator(0, 90, 0),
			FVector(X * XScale, y * YScale, 0)
		);
		Wall->AddInstance(TransformTop);
		Wall->AddInstance(TransformBottom);
	}
}

// Called when the game starts or when spawned
void ADungeonRoom::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ADungeonRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
