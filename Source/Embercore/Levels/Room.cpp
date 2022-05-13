// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"


// Sets default values
ARoom::ARoom(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FloorComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("FloorComponent"));
	FloorComponent->SetupAttachment(RootComponent);
	WallComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("WallComponent"));
	WallComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARoom::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ARoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ARoom::Generate() {
	const float MaxX = RoomWidth * FloorWidth;
	const float MaxY = RoomLength * FloorLength;
	for (float x = 0.0; x < MaxX; x += FloorWidth) {
		AddWall(x, 0.0, FRotator(0, 0, 0));
		AddWall(x, MaxY, FRotator(0, 0, 0));
		for (float y = 0.0; y < MaxY; y += FloorLength) {
			AddFloor(x, y);
			AddWall(0.1, y, FRotator(0, 90, 0));
			AddWall(MaxX, y, FRotator(0, 90, 0));
		}
	}
}

void ARoom::AddFloor(float x, float y) {
	FString Name = FString::Printf(TEXT("Floor(%3.3f,%3.3f)"), x, y);
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(
		RootComponent, UStaticMeshComponent::StaticClass(), FName(*Name));
	StaticMeshComponent->SetStaticMesh(FloorMesh);
	StaticMeshComponent->SetMaterial(0, FloorMaterial);
	StaticMeshComponent->SetRelativeLocation(FVector(x, y, 0.0f));
	StaticMeshComponent->SetupAttachment(FloorComponent);
	StaticMeshComponent->RegisterComponent();
}

void ARoom::AddWall(float x, float y, FRotator Rotator) {
	FString Name = FString::Printf(TEXT("Wall(%3.3f,%3.3f)"), x, y);
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(
		RootComponent, UStaticMeshComponent::StaticClass(), FName(*Name));
	StaticMeshComponent->SetStaticMesh(WallMesh);
	StaticMeshComponent->SetMaterial(0, WallMaterial);
	StaticMeshComponent->SetRelativeLocation(FVector(x, y, 0.0f));
	StaticMeshComponent->SetRelativeRotation(Rotator);
	StaticMeshComponent->SetupAttachment(WallComponent);
	StaticMeshComponent->RegisterComponent();
}


float ARoom::GetRandomFloat(const float Max, const float Min = 0.0) {
	return Min + static_cast<float>(rand()) / (RAND_MAX / (Max - Min));
}
