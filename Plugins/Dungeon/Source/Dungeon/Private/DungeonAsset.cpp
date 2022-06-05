// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonAsset.h"

UDungeonAsset::UDungeonAsset() {
	CurrentStream = FRandomStream();
}

void UDungeonAsset::SetStream(FRandomStream InStream) {
	this->CurrentStream = InStream;
}

void UDungeonAsset::Reset() {
	Nodes.Empty();
}

void UDungeonAsset::GenerateMap() {
	Reset();
	SplitDungeon(Depth, FDungeonContainer(0, 0, Size, Size), -1);
	GenerateRooms(0);
}

void UDungeonAsset::GenerateMapWithStream(FRandomStream InStream = FRandomStream()) {
	SetStream(InStream);
	Reset();
	SplitDungeon(Depth, FDungeonContainer(0, 0, Size, Size), -1);
	GenerateRooms(0);
}

int32 UDungeonAsset::SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex) {
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

TArray<FDungeonContainer> UDungeonAsset::SplitDungeonContainer(FDungeonContainer Container) {
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

bool UDungeonAsset::SplitHorizontal(FDungeonContainer Container) {
	if (Container.Width / Container.Height >= 1.25) {
		return false;
	}
	if (Container.Height / Container.Width >= 1.25) {
		return true;
	}
	return CurrentStream.FRandRange(0, 1) > 0.5f;
}

float UDungeonAsset::RandomPosition(float In) {
	return CurrentStream.FRandRange(In * 0.3f, In * 0.5f);
}

void UDungeonAsset::GenerateRooms(int32 Index) {
	FSubDungeon& Node = Nodes[Index];
	if (Node.IsLeaf()) {
		float RandomX = CurrentStream.FRandRange(MinRoomDelta, Node.Container.Width / 4);
		float RandomY = CurrentStream.FRandRange(MinRoomDelta, Node.Container.Height / 4);
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

FDungeonContainer UDungeonAsset::GetRoomFor(int32 Index) {
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

void UDungeonAsset::IterateNodes(int32 Index = 0) {
	IterateNodesDelegate.ExecuteIfBound(Nodes[Index], Nodes[Index].IsLeaf());
	if (Nodes[Index].Left != -1) { IterateNodes(Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { IterateNodes(Nodes[Index].Right); }
}

FVector UDungeonAsset::GetCenter() const {
	const int32 MapX = Size * FloorWidth;
	const int32 MapY = Size * FloorHeight;
	return FVector(MapX / 2, MapY / 2, 0);
}

FVector UDungeonAsset::GetScale() const {
	return FVector(Size, Size, 1);
}

FVector UDungeonAsset::GetRandomPointFrom(FDungeonContainer Room) {
	return FVector(CurrentStream.RandRange(Room.X + 1, Room.GetXMax() - 1),
	               CurrentStream.RandRange(Room.Y + 1, Room.GetYMax() - 1), 0);
}