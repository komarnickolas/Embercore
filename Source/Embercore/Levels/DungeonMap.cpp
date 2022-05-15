// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

UDungeonMap::UDungeonMap() {
}

void UDungeonMap::DrawDebug() {
	DrawDebugNode(0);
}

void UDungeonMap::DrawDebugNode(int32 NodeIndex) {
	const FDungeonTree Node = Nodes[NodeIndex];
	float X = Node.Container.X;
	float Y = Node.Container.Y;
	float XMax = Node.Container.X + Node.Container.W;
	float YMax = Node.Container.Y + Node.Container.Z;
	DrawDebugLine(GetWorld(), FVector(X, Y, 0), FVector(XMax, Y, 0), FColor::Green);
	DrawDebugLine(GetWorld(), FVector(XMax, Y, 0), FVector(XMax, YMax, 0), FColor::Green);
	DrawDebugLine(GetWorld(), FVector(X, YMax, 0), FVector(XMax, YMax, 0), FColor::Green);
	DrawDebugLine(GetWorld(), FVector(X, Y, 0), FVector(X, YMax, 0), FColor::Green);
	if (Node.Left != 0) { DrawDebugNode(Node.Left); }
	if (Node.Right != 0) { DrawDebugNode(Node.Right); }
}

void UDungeonMap::GenerateMap(FRandomStream InStream) {
	this->Stream = InStream;
	SplitDungeon(Depth, FVector4(0, 0, Size, Size));
	UE_LOG(LogTemp, Warning, TEXT("Dungeon Generated"));
}

TArray<FVector4> UDungeonMap::SplitDungeonContainer(FVector4 Container) {
	TArray<FVector4> Array;
	FVector4 C1, C2;
	const FVector2D SplitPosition = RandomPosition(Container);
	if (SplitPosition.X == 0) {
		C1 = FVector4(Container.X, Container.Y, Stream.FRandRange(Container.Z * 0.3f, Container.Z * 0.5f), Container.W);
		C2 = FVector4(Container.X, Container.Y + C1.Z, Container.Z - C1.Z, Container.W);
	}
	else {
		C1 = FVector4(Container.X, Container.Y, Container.Z, Stream.FRandRange(Container.W * 0.3f, Container.W * 0.5f));
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

FVector2D UDungeonMap::RandomPosition(FVector4 Container) {
	if (Stream.RandRange(0, 1) == 0) {
		return FVector2D(Stream.RandRange(0, Container.Z), 0);
	}
	return FVector2D(0, Stream.RandRange(0, Container.W));
}
