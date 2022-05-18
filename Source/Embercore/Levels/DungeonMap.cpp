// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

#include "LineTypes.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

UDungeonMap::UDungeonMap() {
}

void UDungeonMap::DrawDebug() {
	if (!IsValid(this)) { return; }
	DrawDebugNode(0);
	for (FRectInt Corridor : Corridors) {
		DrawDebugContainer(Corridor, FColor::Blue, 10);
	}
}

FRectInt UDungeonMap::GetStartingRoom() {
	return GetRoomFor(0);
}

FRectInt UDungeonMap::GetEndingRoom() {
	return GetRoomFor(Nodes.Num() - 1);
}

void UDungeonMap::DrawDebugContainer(FRectInt Container, FColor Color, float Z = 0) {
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Scale, Container.ToXMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToXMaxVector(Z) * Scale, Container.ToMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToYMaxVector(Z) * Scale, Container.ToMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Scale, Container.ToYMaxVector(Z) * Scale, Color);
}

void UDungeonMap::DrawDebugNode(int32 NodeIndex) {
	FSubDungeon& Node = Nodes[NodeIndex];
	DrawDebugContainer(Node.Container, FColor::Green, NodeIndex);
	if (Node.IsLeaf()) {
		DrawDebugContainer(Node.Room, FColor::Red, NodeIndex);
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
	if (ParentIndex != -1) { Nodes[Index].Depth = Nodes[ParentIndex].Depth + 1; }
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
	FSubDungeon& Node = Nodes[Index];
	if (Node.IsLeaf()) {
		float RandomX = Stream.FRandRange(MinRoomDelta, Node.Container.Width / 4);
		float RandomY = Stream.FRandRange(MinRoomDelta, Node.Container.Height / 4);
		int32 RoomX = Node.Container.X + RandomX;
		int32 RoomY = Node.Container.Y + RandomY;
		int32 RoomWidth = Node.Container.Width - (RandomX * Stream.FRandRange(1, 2));
		int32 RoomHeight = Node.Container.Height - (RandomY * Stream.FRandRange(1, 2));
		Node.Room = FRectInt(RoomX, RoomY, RoomWidth, RoomHeight);
	}
	if (Node.Left != -1) { GenerateRooms(Node.Left); }
	if (Node.Right != -1) { GenerateRooms(Node.Right); }
	if (!Node.IsLeaf()) {
		GenerateCorridorBetween(Nodes[Index].Left, Nodes[Index].Right);
	}
}

FRectInt UDungeonMap::GetRoomFor(int32 Index) {
	if (Nodes[Index].IsLeaf()) {
		return Nodes[Index].Room;
	}
	FRectInt LeftRoom, RightRoom;
	if (Nodes[Index].Left != -1) {
		LeftRoom = GetRoomFor(Nodes[Index].Left);
	}
	if (Nodes[Index].Right != -1) {
		RightRoom = GetRoomFor(Nodes[Index].Right);
	}
	if (LeftRoom.X > 0) { return LeftRoom; }
	if (RightRoom.X > 0) { return RightRoom; }
	return FRectInt(-1, -1, 0, 0);
}

void UDungeonMap::GenerateCorridorBetween(int32 LeftIndex, int32 RightIndex) {
	FRectInt LeftRoom = GetRoomFor(LeftIndex);
	FRectInt RightRoom = GetRoomFor(RightIndex);
	FVector LeftPoint = GetRandomPointFrom(LeftRoom);
	FVector RightPoint = GetRandomPointFrom(RightRoom);
	if (LeftPoint.X > RightPoint.X) {
		FVector Temp = LeftPoint;
		LeftPoint = RightPoint;
		RightPoint = Temp;
	}
	int32 Width = LeftPoint.X - RightPoint.X;
	int32 Height = LeftPoint.Y - RightPoint.Y;
	if (Width != 0) {
		if (Stream.FRandRange(0, 1) > 0.5) {
			Corridors.Add(FRectInt(LeftPoint.X, LeftPoint.Y, abs(Width + 1), 1));
			if (Height < 0) {
				Corridors.Add(FRectInt(RightPoint.X, LeftPoint.Y, 1, abs(Height)));
			}
			else {
				Corridors.Add(FRectInt(RightPoint.X, LeftPoint.Y, 1, -abs(Height)));
			}
		}
		else {
			if (Height < 0) {
				Corridors.Add(FRectInt(LeftPoint.X, LeftPoint.Y, 1, abs(Height)));
			}
			else {
				Corridors.Add(FRectInt(LeftPoint.X, RightPoint.Y, 1, abs(Height)));
			}

			Corridors.Add(FRectInt(LeftPoint.X, RightPoint.Y, abs(Width) + 1, 1));
		}
	}
	else {
		if (Height < 0) {
			Corridors.Add(FRectInt(LeftPoint.X, LeftPoint.Y, 1, abs(Height)));
		}
		else {
			Corridors.Add(FRectInt(RightPoint.X, RightPoint.Y, 1, abs(Height)));
		}
	}
}

FVector UDungeonMap::GetRandomPointFrom(FRectInt Room) {
	return FVector(Stream.FRandRange(Room.X + 1, Room.GetXMax() - 1),
	               Stream.FRandRange(Room.Y + 1, Room.GetYMax() - 1), 0);
}

void UDungeonMap::IterateNodes(FIterateNodes Functor, int32 Index) {
	Functor.ExecuteIfBound(Nodes[Index]);
	if (Nodes[Index].Left != -1) { IterateNodes(Functor, Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { IterateNodes(Functor, Nodes[Index].Right); }
}

void UDungeonMap::IterateRoom(FIterateRect Iterator, FIterateRect XIterator, FIterateRect YIterator, FRectInt Rect) {
	IterateEntireRoom(Iterator, Rect);
	IterateRoomX(XIterator, Rect);
	IterateRoomY(YIterator, Rect);
}

void UDungeonMap::IterateEntireRoom(FIterateRect Iterator, FRectInt Rect) {
	for (float x = Rect.X; x < Rect.GetXMax(); x++) {
		for (float y = Rect.Y; y < Rect.GetYMax(); y++) {
			Iterator.ExecuteIfBound(x, y);
		}
	}
}

void UDungeonMap::IterateRoomX(FIterateRect Iterator, FRectInt Rect) {
	for (float x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.Y);
	}
	for (float x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.GetYMax());
	}
}

void UDungeonMap::IterateRoomY(FIterateRect Iterator, FRectInt Rect) {
	for (float y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.X, y);
	}
	for (float y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.GetXMax(), y);
	}
}
