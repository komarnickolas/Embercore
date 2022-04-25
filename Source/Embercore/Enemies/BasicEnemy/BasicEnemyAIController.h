// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API ABasicEnemyAIController : public AAIController {
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	UFUNCTION()
	void RandomPatrol();
private:
	class UNavigationSystemV1* NavArea;
	FVector RandomLocation;
};
