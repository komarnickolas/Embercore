// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonBSP.h"
#include "DrawDebugHelpers.h"

UDungeonBSP::UDungeonBSP() {
	CurrentStream = FRandomStream();
}

void UDungeonBSP::SetStream(FRandomStream InStream) {
	this->CurrentStream = InStream;
}

void UDungeonBSP::Reset() {
	Nodes.Empty();
}

void UDungeonBSP::GenerateMap() {
	Reset();
	SplitDungeon(Depth, FDungeonContainer(0, 0, Size, Size), -1);
	GenerateRooms(0);
}

void UDungeonBSP::GenerateMapWithStream(FRandomStream InStream = FRandomStream()) {
	SetStream(InStream);
	Reset();
	SplitDungeon(Depth, FDungeonContainer(0, 0, Size, Size), -1);
	GenerateRooms(0);
}

int32 UDungeonBSP::SplitDungeon(int32 iteration, FDungeonContainer Container, int32 ParentIndex) {
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

TArray<FDungeonContainer> UDungeonBSP::SplitDungeonContainer(FDungeonContainer Container) {
	TArray<FDungeonContainer> Array;
	FDungeonContainer C1, C2;
	if (SplitHorizontal(Container)) {
		C1 = FDungeonContainer(Container.Min.X, Container.Min.Y, Container.Width, RandomPosition(Container.Height));
		C2 = FDungeonContainer(Container.Min.X, Container.Min.Y + C1.Height, Container.Width,
		                       Container.Height - C1.Height);
	}
	else {
		C1 = FDungeonContainer(Container.Min.X, Container.Min.Y, RandomPosition(Container.Width), Container.Height);
		C2 = FDungeonContainer(Container.Min.X + C1.Width, Container.Min.Y, Container.Width - C1.Width,
		                       Container.Height);
	}
	Array.Add(C1);
	Array.Add(C2);
	return Array;
}

bool UDungeonBSP::SplitHorizontal(FDungeonContainer Container) {
	if (Container.Width / Container.Height >= 1.25) {
		return false;
	}
	if (Container.Height / Container.Width >= 1.25) {
		return true;
	}
	return CurrentStream.FRandRange(0, 1) > 0.5f;
}

float UDungeonBSP::RandomPosition(float In) {
	return CurrentStream.FRandRange(In * 0.3f, In * 0.5f);
}

void UDungeonBSP::GenerateRooms(int32 Index) {
	FSubDungeon& Node = Nodes[Index];
	if (Node.IsLeaf()) {
		const float RandomX = CurrentStream.FRandRange(MinRoomDelta, Node.Container.Width / 4);
		const float RandomY = CurrentStream.FRandRange(MinRoomDelta, Node.Container.Height / 4);
		const int32 RoomX = Node.Container.Min.X + RandomX;
		const int32 RoomY = Node.Container.Min.Y + RandomY;
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

FDungeonContainer UDungeonBSP::GetRoomFor(int32 Index) {
	if (Nodes[Index].IsLeaf()) { return Nodes[Index].Room; }
	FDungeonContainer LeftRoom, RightRoom;
	if (Nodes[Index].Left != -1) { LeftRoom = GetRoomFor(Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { RightRoom = GetRoomFor(Nodes[Index].Right); }
	if (LeftRoom.Min.X != 0) { return LeftRoom; }
	if (RightRoom.Min.X != 0) { return RightRoom; }
	return FDungeonContainer(-1, -1, 0, 0);
}

void UDungeonBSP::IterateNodes(FIterateNodesDelegate IterateNodesDelegate, int32 Index = 0) {
	IterateNodesDelegate.ExecuteIfBound(Nodes[Index], Nodes[Index].IsLeaf());
	if (Nodes[Index].Left != -1) { IterateNodes(IterateNodesDelegate, Nodes[Index].Left); }
	if (Nodes[Index].Right != -1) { IterateNodes(IterateNodesDelegate, Nodes[Index].Right); }
}

FVector UDungeonBSP::GetCenter() const {
	return GetScale() / 2;
}

FVector UDungeonBSP::GetScale() const {
	return FVector(Size, Size, 1);
}

void UDungeonBSP::SetContext(UObject* InContext) {
	Context = InContext;
}

void UDungeonBSP::DrawDebug() {
	if (ShowDebug) {
		FIterateNodesDelegate IterateNodesDelegate;
		IterateNodesDelegate.BindDynamic(this, &UDungeonBSP::DrawDebugNode);
		IterateNodes(IterateNodesDelegate, 0);
	}
}

void UDungeonBSP::DrawDebugNode(FSubDungeon Node, bool IsLeaf) {
	DrawDebugContainer(Node.Container, FColor::Green, 100);
	if (IsLeaf) { DrawDebugContainer(Node.Room, FColor::Red, 200); }
}

void UDungeonBSP::DrawDebugContainer(FDungeonContainer Container, FColor Color, float Z = 0) {
	if (Context && Context->GetWorld()) {
		DrawDebugLine(Context->GetWorld(), Scale(Container.Min), Scale(Container.XMax), Color);
		DrawDebugLine(Context->GetWorld(), Scale(Container.XMax), Scale(Container.Max), Color);
		DrawDebugLine(Context->GetWorld(), Scale(Container.YMax), Scale(Container.Max), Color);
		DrawDebugLine(Context->GetWorld(), Scale(Container.Min), Scale(Container.YMax), Color);
	}
}

float UDungeonBSP::ScaleX(float InX) {
	return InX * Theme->FloorWidth;
}

float UDungeonBSP::ScaleY(float InY) {
	return InY * Theme->FloorHeight;
}

FVector UDungeonBSP::Scale(FVector2D Vector, float InZ) {
	return FVector(ScaleX(Vector.X), ScaleY(Vector.Y), InZ);
}
