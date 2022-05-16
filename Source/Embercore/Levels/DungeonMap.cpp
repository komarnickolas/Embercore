// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

UDungeonMap::UDungeonMap() {
}

void UDungeonMap::DrawDebug() {
	DrawDebugNode(0);
}

void UDungeonMap::DrawDebugContainer(FVector4 Container, FColor Color, float Z = 0) {
	const float X = Container.X;
	const float Y = Container.Y;
	const float XMax = Container.X + Container.W;
	const float YMax = Container.Y + Container.Z;
	DrawDebugLine(GetWorld(), FVector(X, Y, Z), FVector(XMax, Y, Z), Color);
	DrawDebugLine(GetWorld(), FVector(XMax, Y, Z), FVector(XMax, YMax, Z), Color);
	DrawDebugLine(GetWorld(), FVector(X, YMax, Z), FVector(XMax, YMax, Z), Color);
	DrawDebugLine(GetWorld(), FVector(X, Y, Z), FVector(X, YMax, Z), Color);

}

void UDungeonMap::DrawDebugNode(int32 NodeIndex) {
	const FDungeonTree Node = Nodes[NodeIndex];
	DrawDebugContainer(Node.Container, FColor::Green, 0);
	DrawDebugContainer(Node.Room, FColor::Red, 1);
	if (Node.Left != -1) { DrawDebugNode(Node.Left); }
	if (Node.Right != -1) { DrawDebugNode(Node.Right); }
}

void UDungeonMap::GenerateMap(FRandomStream InStream) {
	this->Stream = InStream;
	SplitDungeon(Depth, FVector4(0, 0, Size, Size));
	GenerateRooms(0);
	UE_LOG(LogTemp, Warning, TEXT("Dungeon Generated"));
}

TArray<FVector4> UDungeonMap::SplitDungeonContainer(FVector4 Container) {
	TArray<FVector4> Array;
	FVector4 C1, C2;
	if (Stream.FRandRange(0, 1) >= 0.5f) {
		C1 = FVector4(Container.X, Container.Y, RandomPosition(Container.Z), Container.W);
		C2 = FVector4(Container.X, Container.Y + C1.Z, Container.Z - C1.Z, Container.W);
	}
	else {
		C1 = FVector4(Container.X, Container.Y, Container.Z, RandomPosition(Container.W));
		C2 = FVector4(Container.X + C1.W, Container.Y, Container.Z, Container.W - C1.W);
	}
	Array.Add(C1);
	Array.Add(C2);
	return Array;
}

int32 UDungeonMap::SplitDungeon(int32 iteration, FVector4 Container) {
	const int32 Index = Nodes.Add(FDungeonTree(Container));
	if (iteration == 0) { return Index; }

	TArray<FVector4> SplitContainers = SplitDungeonContainer(Container);
	Nodes[Index].Left = SplitDungeon(iteration - 1, SplitContainers[0]);
	Nodes[Index].Right = SplitDungeon(iteration - 1, SplitContainers[1]);
	return Index;
}

float UDungeonMap::RandomPosition(float In) {
	return Stream.FRandRange(In * 0.3f, In * 0.5f);
}

void UDungeonMap::GenerateRooms(int32 Index) {
	FDungeonTree Node = Nodes[Index];
	if (Node.IsLeaf()) {
		const float RandomX = Stream.FRandRange(MinRoomDelta, Node.Container.W / 4);
		const float RandomY = Stream.RandRange(MinRoomDelta, Node.Container.Z / 4);
		Nodes[Index].Room.X = Node.Container.X + RandomX;
		Nodes[Index].Room.Y = Node.Container.Y + RandomY;
		Nodes[Index].Room.W = Node.Container.W - (RandomX * Stream.FRandRange(1, 2));
		Nodes[Index].Room.Z = Node.Container.Z - (RandomY * Stream.FRandRange(1, 2));
	}
	else {
		if (Node.Left != -1) { GenerateRooms(Node.Left); }
		if (Node.Right != -1) { GenerateRooms(Node.Right); }
	}
}
