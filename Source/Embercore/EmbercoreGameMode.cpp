// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercoreGameMode.h"
#include "Engine/World.h"
#include "EmbercorePlayerController.h"
#include "EmbercoreCharacter.h"
#include "EmbercorePlayerHUD.h"
#include "EmbercorePlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AEmbercoreGameMode::AEmbercoreGameMode() {
	// use our custom PlayerController class
	PlayerControllerClass = AEmbercorePlayerController::StaticClass();
	PlayerStateClass = AEmbercorePlayerState::StaticClass();
}

void AEmbercoreGameMode::PlayerDied(AController* Controller) {
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(
		SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AEmbercoreGameMode::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	AEmbercorePlayerController* PC = Cast<AEmbercorePlayerController>(Controller);
	if (PC) {
		PC->SetRespawnCountdown(RespawnDelay);
	}
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

void AEmbercoreGameMode::RespawnPlayer(AController* Controller) {
	if (Controller->IsPlayerController()) {
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APlayerCharacter* Player = GetWorld()->SpawnActor<APlayerCharacter>(
			PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Player);
	}
	else {
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APlayerCharacter* Player = GetWorld()->SpawnActor<APlayerCharacter>(
			PlayerClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Player);
	}
}
