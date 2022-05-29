﻿// Fill out your copyright notice in the Description page of Project Settings.


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
// Called every frame
void ADungeonMap::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	DrawDebug();
}


void ADungeonMap::DrawDebug() {
	if (ShowDebug) { DrawDebugNode(0); }
}

FVector ADungeonMap::GetCenterOfRect(FDungeonContainer RectInt) {
	return RectInt.GetCenter();
}

FDungeonContainer ADungeonMap::GetStartingRoom() {
	return FDungeonContainer(0, -5, 3, 2);
}

FDungeonContainer ADungeonMap::GetEndingRoom() {
	return FDungeonContainer(0, -1, 2, 3);
}

void ADungeonMap::DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z = 0) {
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * FloorWidth, Container.ToXMaxVector(Z) * FloorWidth, Color);
	DrawDebugLine(GetWorld(), Container.ToXMaxVector(Z) * FloorHeight, Container.ToMaxVector(Z) * FloorHeight, Color);
	DrawDebugLine(GetWorld(), Container.ToYMaxVector(Z) * FloorWidth, Container.ToMaxVector(Z) * FloorWidth, Color);
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * FloorHeight, Container.ToYMaxVector(Z) * FloorHeight, Color);
}

void ADungeonMap::DrawDebugNode(int32 NodeIndex) {
	FSubDungeon& Node = Nodes[NodeIndex];
	DrawDebugContainer(Node.Container, FColor::Green, 100);
	if (Node.IsLeaf()) {
		DrawDebugContainer(Node.Room, FColor::Red, 200);
	}
	if (Node.Left != -1) {
		DrawDebugNode(Node.Left);
	}
	if (Node.Right != -1) {
		DrawDebugNode(Node.Right);
	}
}

void ADungeonMap::GenerateMap(FRandomStream InStream) {
	this->Stream = InStream;
	this->Nodes.Empty();
	SplitDungeon(Depth, FDungeonContainer(0, 0, Size, Size), -1);
	GenerateRooms(0);
	UE_LOG(LogTemp, Warning, TEXT("Dungeon Generated"));
}

bool ADungeonMap::SplitHorizontal(FDungeonContainer Container) {
	if (Container.Width / Container.Height >= 1.25) {
		return false;
	}
	if (Container.Height / Container.Width >= 1.25) {
		return true;
	}
	return Stream.FRandRange(0, 1) > 0.5f;
}

TArray<FDungeonContainer> ADungeonMap::SplitDungeonContainer(FDungeonContainer Container) {
	TArray<FDungeonContainer> Array;
	FDungeonContainer C1, C2;
	if (SplitHorizontal(Container)) {
		C1 = FDungeonContainer(Container.X, Container.Y, Container.Width, RandomPosition(Container.Height));
		C2 = FDungeonContainer(Container.X, Container.Y + C1.Height, Container.Width, Container.Height - C1.Height);
	}
	else {
		C1 = FDungeonContainer(Container.X, Container.Y, RandomPosition(Container.Width), Container.Height);
		C2 = FDungeonContainer(Container.X + C1.Width, Container.Y, Container.Width - C1.Width, Container.Height);
	}
	Array.Add(C1);
	Array.Add(C2);
	return Array;
}

int32 ADungeonMap::SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex) {
	const int32 Index = Nodes.Add(FSubDungeon(Container));
	Nodes[Index].Index = Index;
	Nodes[Index].Parent = ParentIndex;
	if (ParentIndex != -1) { Nodes[Index].Depth = Nodes[ParentIndex].Depth + 1; }
	if (iteration == 0) { return Index; }

	if (Container.Width / 2 >= MinRoomWidth && Container.Height / 2 >= MinRoomHeight) {
		TArray<FDungeonContainer> SplitContainers = SplitDungeonContainer(Container);
		while (SplitContainers[0].Width < MinRoomWidth ||
			SplitContainers[0].Height < MinRoomHeight ||
			SplitContainers[1].Width < MinRoomWidth ||
			SplitContainers[1].Height < MinRoomHeight) {
			SplitContainers = SplitDungeonContainer(Container);
		}
		Nodes[Index].Left = SplitDungeon(iteration - 1, SplitContainers[0], Index);
		Nodes[Index].Right = SplitDungeon(iteration - 1, SplitContainers[1], Index);
	}
	return Index;
}

float ADungeonMap::RandomPosition(float In) {
	return Stream.FRandRange(In * 0.3f, In * 0.5f);
}

void ADungeonMap::GenerateRooms(int32 Index) {
	FSubDungeon& Node = Nodes[Index];
	if (Node.IsLeaf()) {
		float RandomX = Stream.FRandRange(MinRoomDelta, Node.Container.Width / 4);
		float RandomY = Stream.FRandRange(MinRoomDelta, Node.Container.Height / 4);
		int32 RoomX = Node.Container.X + RandomX;
		int32 RoomY = Node.Container.Y + RandomY;
		int32 RoomWidth = Node.Container.Width - RandomX;
		int32 RoomHeight = Node.Container.Height - RandomY;
		if (RoomWidth < MinRoomWidth) {
			if (Node.Container.Width > MinRoomWidth) {
				RoomWidth = MinRoomWidth;
			}
		}
		if (RoomHeight < MinRoomHeight) {
			if (Node.Container.Height > MinRoomHeight) {
				RoomHeight = MinRoomHeight;
			}
		}
		if (RoomHeight >= MinRoomHeight && RoomWidth >= MinRoomWidth) {
			Node.Room = FDungeonContainer(RoomX, RoomY, RoomWidth, RoomHeight);
		}
	}
	if (Node.Left != -1) { GenerateRooms(Node.Left); }
	if (Node.Right != -1) { GenerateRooms(Node.Right); }
}

FDungeonContainer ADungeonMap::GetRoomFor(int32 Index) {
	if (Nodes[Index].IsLeaf()) {
		return Nodes[Index].Room;
	}
	FDungeonContainer LeftRoom, RightRoom;
	if (Nodes[Index].Left != -1) {
		LeftRoom = GetRoomFor(Nodes[Index].Left);
	}
	if (Nodes[Index].Right != -1) {
		RightRoom = GetRoomFor(Nodes[Index].Right);
	}
	if (LeftRoom.X != 0) { return LeftRoom; }
	if (RightRoom.X != 0) { return RightRoom; }
	return FDungeonContainer(-1, -1, 0, 0);
}

FVector ADungeonMap::SpawnVectorFor(FDungeonContainer Room, float InZ) {
	return FVector(Room.X * FloorWidth, Room.Y * FloorHeight, InZ);
}

ADungeonRoom* ADungeonMap::SpawnRoom(UClass* InClass, FDungeonContainer Room) {
	if (Room.X == 0 || Room.Y == 0) { return nullptr; }
	FTransform SpawnTransform(FRotator(0, 0, 0), SpawnVectorFor(Room, 0));
	ADungeonRoom* DeferredRoom = Cast<ADungeonRoom>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InClass, SpawnTransform));
	if (DeferredRoom != nullptr) {
		DeferredRoom->SetupRoom(Room.Width, Room.Height,
		                        FloorWidth, FloorHeight,
		                        FloorMesh, FloorMaterial,
		                        WallMesh, WallMaterial);
		DeferredRoom->SetOwner(this);
		UGameplayStatics::FinishSpawningActor(DeferredRoom, SpawnTransform);
	}
	return DeferredRoom;
}

void ADungeonMap::SpawnNode(UClass* InClass, int32 Index) {
	if (Nodes[Index].IsLeaf()) {
		ADungeonRoom* Room = SpawnRoom(InClass, Nodes[Index].Room);
		if (Room) { Room->Index = Index; }
	}
	if (Nodes[Index].Left != -1) { SpawnNode(InClass, Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { SpawnNode(InClass, Nodes[Index].Right); }
}

void ADungeonMap::SpawnMap(UClass* InClass) {
	if (Nodes.Num() > 0) { SpawnNode(InClass, 0); }
}

FVector ADungeonMap::GetRandomPointFrom(FDungeonContainer Room) {
	return FVector(Stream.RandRange(Room.X + 1, Room.GetXMax() - 1),
	               Stream.RandRange(Room.Y + 1, Room.GetYMax() - 1), 0);
}

bool ADungeonMap::IsLeaf(FSubDungeon SubDungeon) {
	return SubDungeon.IsLeaf();
}

FVector ADungeonMap::GetMapCenter() {
	int32 MapX = Size * FloorWidth;
	int32 MapY = Size * FloorHeight;
	return FVector(MapX / 2, MapY / 2, 0);
}

void ADungeonMap::IterateNodes(FIterateNodes Functor, int32 Index) {
	Functor.ExecuteIfBound(Nodes[Index], Nodes[Index].IsLeaf());
	if (Nodes[Index].Left != -1) { IterateNodes(Functor, Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { IterateNodes(Functor, Nodes[Index].Right); }
}

void ADungeonMap::IterateLeafs(FIterateLeafs Functor, int32 Index) {
	if (Nodes[Index].IsLeaf()) {
		Functor.ExecuteIfBound(Nodes[Index]);
	}
	if (Nodes[Index].Left != -1) { IterateLeafs(Functor, Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { IterateLeafs(Functor, Nodes[Index].Right); }
}

void ADungeonMap::IterateRoom(FIterateRect Iterator, FIterateRect XIterator, FIterateRect YIterator,
                              FDungeonContainer Rect) {
	IterateEntireRoom(Iterator, Rect);
	IterateRoomX(XIterator, Rect);
	IterateRoomY(YIterator, Rect);
}

void ADungeonMap::IterateEntireRoom(FIterateRect Iterator, FDungeonContainer Rect) {
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
			Iterator.ExecuteIfBound(x, y);
		}
	}
}

void ADungeonMap::IterateRoomX(FIterateRect Iterator, FDungeonContainer Rect) {
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.Y);
	}
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.GetYMax());
	}
}

void ADungeonMap::IterateRoomY(FIterateRect Iterator, FDungeonContainer Rect) {
	for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.X, y);
	}
	for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.GetXMax(), y);
	}
}
