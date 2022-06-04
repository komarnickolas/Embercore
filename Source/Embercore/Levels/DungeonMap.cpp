// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

#include "MathUtil.h"
#include "AI/NavigationSystemBase.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

ADungeonMap::ADungeonMap() {
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Root")));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetHiddenInGame(true);
}

void ADungeonMap::ConstructStaticMesh() {
	StaticMeshComponent->SetStaticMesh(Dungeon->FloorMesh);
	StaticMeshComponent->SetMaterial(0, Dungeon->FloorMaterial);
	StaticMeshComponent->SetRelativeScale3D(Dungeon->GetScale());
}

// Called every frame
void ADungeonMap::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	DrawDebug();
}


void ADungeonMap::DrawDebug() {
	if (ShowDebug) {
		Dungeon->IterateNodesDelegate.Clear();
		Dungeon->IterateNodesDelegate.BindDynamic(this, &ADungeonMap::DrawDebugNode);
		Dungeon->IterateNodes(0);
	}
}

void ADungeonMap::DrawDebugNode(FSubDungeon Node, bool IsLeaf) {
	DrawDebugContainer(Node.Container, FColor::Green, 100);
	if (IsLeaf) { DrawDebugContainer(Node.Room, FColor::Red, 200); }
}

void ADungeonMap::DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z = 0) {
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Dungeon->FloorWidth,
	              Container.ToXMaxVector(Z) * Dungeon->FloorWidth, Color);
	DrawDebugLine(GetWorld(), Container.ToXMaxVector(Z) * Dungeon->FloorHeight,
	              Container.ToMaxVector(Z) * Dungeon->FloorHeight, Color);
	DrawDebugLine(GetWorld(), Container.ToYMaxVector(Z) * Dungeon->FloorWidth,
	              Container.ToMaxVector(Z) * Dungeon->FloorWidth, Color);
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Dungeon->FloorHeight,
	              Container.ToYMaxVector(Z) * Dungeon->FloorHeight, Color);
}

FVector ADungeonMap::GetCenterOfRect(FDungeonContainer RectInt) {
	return RectInt.GetCenter();
}

FVector ADungeonMap::SpawnVectorFor(FDungeonContainer Room, float InZ) {
	return FVector(Room.X * Dungeon->FloorWidth, Room.Y * Dungeon->FloorHeight, InZ);
}

ADungeonRoom* ADungeonMap::SpawnRoom(UClass* InClass, FDungeonContainer Room) {
	if (Room.X == 0 || Room.Y == 0) { return nullptr; }
	FTransform SpawnTransform(FRotator(0, 0, 0), SpawnVectorFor(Room, 0));
	ADungeonRoom* DeferredRoom = Cast<ADungeonRoom>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InClass, SpawnTransform));
	if (DeferredRoom != nullptr) {
		DeferredRoom->SetupRoom(Room.Width, Room.Height,
		                        Dungeon->FloorWidth, Dungeon->FloorHeight,
		                        Dungeon->FloorMesh, Dungeon->FloorMaterial,
		                        Dungeon->WallMesh, Dungeon->WallMaterial);
		DeferredRoom->SetOwner(this);
		UGameplayStatics::FinishSpawningActor(DeferredRoom, SpawnTransform);
	}
	return DeferredRoom;
}
