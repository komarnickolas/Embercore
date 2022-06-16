// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMesh.h"

#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ADungeonMesh::ADungeonMesh() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot"));
	Floor = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Floor");
	Floor->SetupAttachment(RootComponent);
	Wall = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Wall");
	Wall->SetupAttachment(RootComponent);
}


void ADungeonMesh::DrawMap(FSubDungeon SubDungeon, bool bIsLeaf) {
	if (bIsLeaf) {
		AddFloor(SubDungeon.Room);
		AddWalls(SubDungeon.Room);
	}
}

void ADungeonMesh::AddFloor(FDungeonContainer Room) {
	FVector Location = Dungeon->Scale(Room.Min);
	Floor->AddInstance(FTransform(
		FRotator(0, 0, 0),
		Location,
		FVector(Room.Width, Room.Height, 1)
	));
}

void ADungeonMesh::AddWalls(FDungeonContainer Room) {
	const FVector LocationX = Dungeon->Scale(Room.Min);
	const FVector LocationY = Dungeon->Scale(Room.Max);
	const FVector LocationXMax = Dungeon->Scale(Room.XMax);
	const FVector LocationYMax = Dungeon->Scale(Room.YMax);
	Wall->AddInstance(FTransform(
		FRotator(0, 0, 0),
		LocationX,
		FVector(Room.Width, 1, 1)
	));
	Wall->AddInstance(FTransform(
		FRotator(0, 0, 0),
		LocationYMax,
		FVector(Room.Width, 1, 1)
	));
	Wall->AddInstance(FTransform(
		FRotator(0, 90, 0),
		LocationX,
		FVector(Room.Height, 1, 1)
	));
	Wall->AddInstance(FTransform(
		FRotator(0, 90, 0),
		LocationXMax,
		FVector(Room.Height, 1, 1)
	));
}

// Called when the game starts or when spawned
void ADungeonMesh::BeginPlay() {
	Super::BeginPlay();
	Floor->RemoveInstance(0);
	FIterateNodesDelegate IterateNodesDelegate;
	IterateNodesDelegate.BindDynamic(this, &ADungeonMesh::DrawMap);
	Dungeon->IterateNodes(IterateNodesDelegate, 0);
}

// Called every frame
void ADungeonMesh::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	Dungeon->DrawDebug();
}

void ADungeonMesh::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	if (Dungeon) {
		Dungeon->SetContext(this);
		Dungeon->GenerateMap();
		Dungeon->Theme->SetMeshAsFloor(Floor);
		Dungeon->Theme->SetMeshAsWall(Wall);
		Floor->ClearInstances();
		Floor->AddInstance(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), Dungeon->GetScale()));
	}
}
