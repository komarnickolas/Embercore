// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

#include "LineTypes.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

UDungeonMap::UDungeonMap() {
}

void UDungeonMap::DrawDebug() {
	if (!IsValid(this)) { return; }
	DrawDebugNode(0);
	GenerateCorridors();
}

void UDungeonMap::DrawDebugContainer(FRectInt Container, FColor Color, float Z = 0) {
	DrawDebugLine(GetWorld(), Container.ToVector(Z), Container.ToXMaxVector(Z), Color);
	DrawDebugLine(GetWorld(), Container.ToXMaxVector(Z), Container.ToMaxVector(Z), Color);
	DrawDebugLine(GetWorld(), Container.ToYMaxVector(Z), Container.ToMaxVector(Z), Color);
	DrawDebugLine(GetWorld(), Container.ToVector(Z), Container.ToYMaxVector(Z), Color);
}

void UDungeonMap::DrawDebugNode(int32 NodeIndex) {
	FSubDungeon& Node = Nodes[NodeIndex];
	DrawDebugContainer(Node.Container, FColor::Green);
	if (Node.IsLeaf()) {
		DrawDebugContainer(Node.Room, FColor::Red, 1);
	}
	if (Node.Left != -1) {
		DrawDebugNode(Node.Left);
	}
	if (Node.Right != -1) {
		DrawDebugNode(Node.Right);
	}
}

void UDungeonMap::GenerateMap(FRandomStream InStream) {
	this->Stream = InStream;
	SplitDungeon(Depth, FRectInt(0, 0, Size, Size), -1);
	GenerateRooms(0);
	UE_LOG(LogTemp, Warning, TEXT("Dungeon Generated"));
}

TArray<FRectInt> UDungeonMap::SplitDungeonContainer(FRectInt Container) {
	TArray<FRectInt> Array;
	FRectInt C1, C2;
	if (Stream.FRandRange(0, 1) > 0.5f) {
		C1 = FRectInt(Container.X, Container.Y, Container.Width, RandomPosition(Container.Height));
		C2 = FRectInt(Container.X, Container.Y + C1.Height, Container.Width, Container.Height - C1.Height);
	}
	else {
		C1 = FRectInt(Container.X, Container.Y, RandomPosition(Container.Width), Container.Height);
		C2 = FRectInt(Container.X + C1.Width, Container.Y, Container.Width - C1.Width, Container.Height);
	}
	Array.Add(C1);
	Array.Add(C2);
	return Array;
}

int32 UDungeonMap::SplitDungeon(int32 iteration, FRectInt Container, int32 ParentIndex) {
	const int32 Index = Nodes.Add(FSubDungeon(Container));
	Nodes[Index].Index = Index;
	Nodes[Index].Parent = ParentIndex;
	if (iteration == 0) { return Index; }

	TArray<FRectInt> SplitContainers = SplitDungeonContainer(Container);
	Nodes[Index].Left = SplitDungeon(iteration - 1, SplitContainers[0], Index);
	Nodes[Index].Right = SplitDungeon(iteration - 1, SplitContainers[1], Index);
	return Index;
}

float UDungeonMap::RandomPosition(float In) {
	return Stream.FRandRange(In * 0.3f, In * 0.5f);
}

void UDungeonMap::GenerateRooms(int32 Index) {
	if (Nodes[Index].IsLeaf()) {
		float RandomX = Stream.FRandRange(MinRoomDelta, Nodes[Index].Container.Width / 4);
		float RandomY = Stream.FRandRange(MinRoomDelta, Nodes[Index].Container.Height / 4);
		float RoomX = Nodes[Index].Container.X + RandomX;
		float RoomY = Nodes[Index].Container.Y + RandomY;
		float RoomWidth = Nodes[Index].Container.Width - (RandomX * Stream.FRandRange(1, 2));
		float RoomHeight = Nodes[Index].Container.Height - (RandomY * Stream.FRandRange(1, 2));
		Nodes[Index].Room = FRectInt(RoomX, RoomY, RoomWidth, RoomHeight);
	}
	else {
		if (Nodes[Index].Left != -1) { GenerateRooms(Nodes[Index].Left); }
		if (Nodes[Index].Right != -1) { GenerateRooms(Nodes[Index].Right); }
	}
}

void UDungeonMap::GenerateCorridors() {
	for (const FSubDungeon& Node : Nodes) {
		if (!Node.IsLeaf()) {
			if (Nodes[Node.Left].IsLeaf() && Nodes[Node.Right].IsLeaf()) {
				FRectInt LeftRoom = Nodes[Node.Left].Room;
				FRectInt RightRoom = Nodes[Node.Right].Room;
				FVector LeftPoint = GetRandomPointFrom(LeftRoom);
				FVector RightPoint = GetRandomPointFrom(RightRoom);
				DrawDebugLine(GetWorld(), LeftPoint, RightPoint, FColor::Blue);
			}
		}
	}
}

void UDungeonMap::GenerateCorridorsBetweenChildren(int32 LeftIndex, int32 RightIndex) {
}

FVector UDungeonMap::GetRandomPointFrom(FRectInt Room) {
	return FVector(Stream.FRandRange(Room.X + 1, Room.GetXMax() - 1),
	               Stream.FRandRange(Room.Y + 1, Room.GetYMax() - 1), 0);
}
