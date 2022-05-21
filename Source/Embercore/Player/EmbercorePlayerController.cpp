// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercorePlayerController.h"

#include <string>

#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "EmbercorePlayerState.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AEmbercorePlayerController::AEmbercorePlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AEmbercorePlayerController::BeginPlay() {
	Super::BeginPlay();
}

void AEmbercorePlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (GetCharacter()) {
		FVector MouseLocation, MouseDirection;
		this->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
		const FVector MousePosition = (MouseDirection * 1000) + MouseLocation;
		const FVector CharacterLocation = GetCharacter()->GetActorLocation();
		const FRotator CharacterRotation = GetCharacter()->GetActorRotation();
		const FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, MousePosition);
		const FRotator NewRotation = FRotator(CharacterRotation.Pitch, LookRotation.Yaw, CharacterRotation.Roll);
		GetCharacter()->SetActorRotation(NewRotation);
	}
}

void AEmbercorePlayerController::ShowDamageNumber_Implementation(float DamageAmount,
                                                                 AEmbercoreCharacter* TargetCharacter) {
	if (TargetCharacter) {
		UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), DamageAmount);
	}
}

bool AEmbercorePlayerController::ShowDamageNumber_Validate(float DamageAmount, AEmbercoreCharacter* TargetCharacter) {
	return true;
}

// Server only
void AEmbercorePlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	AEmbercorePlayerState* PS = GetPlayerState<AEmbercorePlayerState>();
	if (PS) {
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}
