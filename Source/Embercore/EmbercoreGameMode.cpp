// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercoreGameMode.h"
#include "Engine/World.h"
#include "Player/EmbercorePlayerController.h"
#include "EmbercoreCharacter.h"
#include "Player/EmbercorePlayerHUD.h"
#include "Player/EmbercorePlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EmbercorePlayer.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AEmbercoreGameMode::AEmbercoreGameMode() {
	// use our custom PlayerController class
	PlayerControllerClass = AEmbercorePlayerController::StaticClass();
	PlayerStateClass = AEmbercorePlayerState::StaticClass();
	HUDClass = AEmbercorePlayerHUD::StaticClass();
	PlayerClass = AEmbercorePlayer::StaticClass();
}

FWeaponDataStructure AEmbercoreGameMode::FindWeapon_Implementation(FName WeaponId, bool& Success) {
	Success = false;

	FWeaponDataStructure WeaponDataStructure;
	if (WeaponDatabase == nullptr) { return WeaponDataStructure; }

	for (int i = 0; i < WeaponDatabase->Data.Num(); i++) {
		if (WeaponDatabase->Data[i].WeaponId == WeaponId) {
			Success = true;
			return WeaponDatabase->Data[i];
		}
	}
	return WeaponDataStructure;
}

void AEmbercoreGameMode::BeginPlay() {
	Super::BeginPlay();

	// Get the enemy hero spawn point
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors) {
		if (Actor->GetName() == FString("EnemyHeroSpawn")) {
			EnemySpawnPoint = Actor;
			break;
		}
	}
}
