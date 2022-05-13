// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorComponent.h"


UFloorComponent::UFloorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFloorComponent::BeginPlay() {
	Super::BeginPlay();
}


// Called every frame
void UFloorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                    FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UStaticMeshComponent* UFloorComponent::CreateFloor(FString Name) {
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(
		this, UStaticMeshComponent::StaticClass(), FName(*Name));
	StaticMeshComponent->SetStaticMesh(FloorMesh);
	StaticMeshComponent->SetMaterial(0, FloorMaterial);
	return StaticMeshComponent;
}

void UFloorComponent::AddFloor(float x, float y) {
	FString Name = FString::Printf(TEXT("Floor(%3.3f,%3.3f)"), x, y);
	UStaticMeshComponent* StaticMeshComponent = CreateFloor(Name);
	StaticMeshComponent->SetRelativeLocation(FVector(x, y, 0.0f));
	StaticMeshComponent->SetupAttachment(this);
	StaticMeshComponent->RegisterComponent();
}

void UFloorComponent::Generate() {
	const float MaxX = Width * (FloorWidth || 1.0);
	const float MaxY = Length * (FloorLength || 1.0);
	for (float x = 0.0; x < MaxX; x += FloorWidth) {
		for (float y = 0.0; y < MaxY; y += FloorLength) {
			AddFloor(x, y);
		}
	}
}
