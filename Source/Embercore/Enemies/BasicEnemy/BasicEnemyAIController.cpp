// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemyAIController.h"
#include "NavigationSystem.h"

void ABasicEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

	RandomPatrol();
}

void ABasicEnemyAIController::RandomPatrol() {
	if (NavArea) {
		NavArea->K2_GetRandomReachablePointInRadius(GetWorld(),
		                                            GetPawn()->GetActorLocation(),
		                                            RandomLocation,
		                                            1500.0f);
		MoveToLocation(RandomLocation);
	}
}
