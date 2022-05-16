// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"


// Sets default values
ARoom::ARoom(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FloorComponent = CreateDefaultSubobject<UFloorComponent>(FName("FloorComponent"));
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
