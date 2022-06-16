// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonTheme.h"

void UDungeonTheme::SetMeshAsFloor(UStaticMeshComponent* MeshComponent) {
	SetMesh(MeshComponent, FloorMesh, FloorMaterial);
}

void UDungeonTheme::SetMeshAsWall(UStaticMeshComponent* MeshComponent) {
	SetMesh(MeshComponent, WallMesh, WallMaterial);
}

void UDungeonTheme::SetMesh(UStaticMeshComponent* MeshComponent, UStaticMesh* Mesh, UMaterial* Material) {
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetMaterial(0, Material);
}
