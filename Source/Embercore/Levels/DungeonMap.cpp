// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonMap.h"

#include "MathUtil.h"

// FVector4(X=X,Y=Y,Z=Height,W=Width)

UDungeonMap::UDungeonMap() {
}

void UDungeonMap::DrawDebug() {
	if (!ShowDebug) { return; }
	if (!IsValid(this)) { return; }
	DrawDebugNode(0);
}

FVector UDungeonMap::GetCenterOfRect(FRectInt RectInt) {
	return RectInt.GetCenter();
}

FRectInt UDungeonMap::GetStartingRoom() {
	return FRectInt(0, -5, 3, 2);
}

FRectInt UDungeonMap::GetEndingRoom() {
	return FRectInt(0, -1, 2, 3);
}

void UDungeonMap::DrawDebugContainer(FRectInt Container, FColor Color, float Z = 0) {
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Scale, Container.ToXMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToXMaxVector(Z) * Scale, Container.ToMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToYMaxVector(Z) * Scale, Container.ToMaxVector(Z) * Scale, Color);
	DrawDebugLine(GetWorld(), Container.ToVector(Z) * Scale, Container.ToYMaxVector(Z) * Scale, Color);
}

void UDungeonMap::DrawDebugNode(int32 NodeIndex) {
	FSubDungeon& Node = Nodes[NodeIndex];
	DrawDebugContainer(Node.Container, FColor::Green, -1);
	if (Node.IsLeaf()) {
		DrawDebugContainer(Node.Room, FColor::Red, -1);
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

bool UDungeonMap::SplitHorizontal(FRectInt Container) {
	if (Container.Width / Container.Height >= 1.25) {
		return false;
	}
	if (Container.Height / Container.Width >= 1.25) {
		return true;
	}
	return Stream.FRandRange(0, 1) > 0.5f;
}

TArray<FRectInt> UDungeonMap::SplitDungeonContainer(FRectInt Container) {
	TArray<FRectInt> Array;
	FRectInt C1, C2;
	if (SplitHorizontal(Container)) {
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

	if (Container.Width / 2 >= MinRoomWidth && Container.Height / 2 >= MinRoomHeight) {
		TArray<FRectInt> SplitContainers = SplitDungeonContainer(Container);
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
			Node.Room = FRectInt(RoomX, RoomY, RoomWidth, RoomHeight);
		}
	}
	if (Node.Left != -1) { GenerateRooms(Node.Left); }
	if (Node.Right != -1) { GenerateRooms(Node.Right); }
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
	if (LeftRoom.X != 0) { return LeftRoom; }
	if (RightRoom.X != 0) { return RightRoom; }
	return FRectInt(-1, -1, 0, 0);
}

FVector UDungeonMap::GetRandomPointFrom(FRectInt Room) {
	return FVector(Stream.RandRange(Room.X + 1, Room.GetXMax() - 1),
	               Stream.RandRange(Room.Y + 1, Room.GetYMax() - 1), 0);
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
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
			Iterator.ExecuteIfBound(x, y);
		}
	}
}

void UDungeonMap::IterateRoomX(FIterateRect Iterator, FRectInt Rect) {
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.Y);
	}
	for (int32 x = Rect.X; x < Rect.GetXMax(); x++) {
		Iterator.ExecuteIfBound(x, Rect.GetYMax());
	}
}

void UDungeonMap::IterateRoomY(FIterateRect Iterator, FRectInt Rect) {
	for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.X, y);
	}
	for (int32 y = Rect.Y; y < Rect.GetYMax(); y++) {
		Iterator.ExecuteIfBound(Rect.GetXMax(), y);
	}
}
