// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom.h"

#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADungeonRoom::ADungeonRoom() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Room")));
	Floor = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Floor"));
	Floor->SetupAttachment(RootComponent);
	Floor->SetIsReplicated(true);
	Wall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Wall"));
	Wall->SetupAttachment(RootComponent);
	Wall->SetIsReplicated(true);
	bReplicates = true;
}

void ADungeonRoom::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonRoom, X);
	DOREPLIFETIME(ADungeonRoom, Y);
	DOREPLIFETIME(ADungeonRoom, XScale);
	DOREPLIFETIME(ADungeonRoom, YScale);
	DOREPLIFETIME(ADungeonRoom, Width);
	DOREPLIFETIME(ADungeonRoom, Height);
	DOREPLIFETIME(ADungeonRoom, Index);
	DOREPLIFETIME(ADungeonRoom, Cleared);
	DOREPLIFETIME(ADungeonRoom, Active);
	DOREPLIFETIME(ADungeonRoom, Floor);
	DOREPLIFETIME(ADungeonRoom, Wall);
}

void ADungeonRoom::OnRep_X() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_X: %i"), Index);
	Fill();
}

void ADungeonRoom::OnRep_Y() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Y: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_XScale() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_XScale: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_YScale() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_YScale: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Width() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Width: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Height() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Height: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Index() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Index: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Cleared() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Cleared: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Active() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Active: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Floor() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Floor: %i"), Index);
	Fill();
};

void ADungeonRoom::OnRep_Wall() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Wall: %i"), Index);
	Fill();
};

void ADungeonRoom::SetupRoom(int32 InX, int32 InY, float InXScale, float InYScale, UStaticMesh* FloorMesh,
                             UMaterial* FloorMaterial, UStaticMesh* WallMesh,
                             UMaterial* WallMaterial) {
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
}

FVector ADungeonRoom::GetRandomPoint() {
	return UKismetMathLibrary::RandomPointInBoundingBox(GetCenter(), GetHalfSize());
}

FVector ADungeonRoom::GetHalfSize() {
	return FVector(Width / 2, Height / 2, 0);
}

FVector ADungeonRoom::GetCenter() {
	FVector Origin = GetActorLocation();
	Origin.X = Origin.X + (Width / 2);
	Origin.Y = Origin.Y + (Height / 2);
	return Origin;
}

// Called when the game starts or when spawned
void ADungeonRoom::BeginPlay() {
	Super::BeginPlay();
}

void ADungeonRoom::Fill() {
	UE_LOG(LogTemp, Warning, TEXT("Fill Room: %i"), Index);
	Width = X * XScale;
	Height = Y * YScale;
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

// Called every frame
void ADungeonRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
