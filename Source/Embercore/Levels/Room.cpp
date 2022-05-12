// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"


// Sets default values
ARoom::ARoom(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	FString Info = FString::Printf(TEXT("%1i,%1i"), RoomWidth, RoomHeight);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Info);
	for (float x = 0.0; x < RoomWidth * FloorWidth; x += FloorWidth) {
		for (float y = 0.0; y < RoomHeight * FloorHeight; y += FloorHeight) {
			FString Name = FString::Printf(TEXT("%3.3f,%3.3f"), x, y);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Name);
			Floor.Add(NewObject<UStaticMeshComponent>(RootComponent, UStaticMeshComponent::StaticClass(),
			                                          FName(*Name)));
			Floor.Last()->SetStaticMesh(FloorMesh);
			Floor.Last()->SetMaterial(0, FloorMaterial);
			Floor.Last()->SetRelativeLocation(FVector(x, y, 0.0f));
			Floor.Last()->SetupAttachment(RootComponent);
			Floor.Last()->RegisterComponent();
		}
	}
}


float ARoom::GetRandomFloat(const float Max, const float Min = 0.0) {
	return Min + static_cast<float>(rand()) / (RAND_MAX / (Max - Min));
}
