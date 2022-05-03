// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercorePlayerController.h"

#include <string>

#include "AbilitySystemComponent.h"
#include "Embercore.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "EmbercoreCharacter.h"
#include "EmbercorePlayerState.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerCharacter.h"
#include "UI/EmbercoreDamageTextWidgetComponent.h"
#include "UI/EmbercoreHUDWidget.h"

AEmbercorePlayerController::AEmbercorePlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AEmbercorePlayerController::CreateHUD() {
	// Only create once
	if (UIHUDWidget) {
		return;
	}

	if (!UIHUDWidgetClass) {
		UE_LOG(LogTemp, Error,
		       TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."),
		       *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController()) {
		return;
	}

	// Need a valid PlayerState to get attributes from
	AEmbercorePlayerState* PS = GetPlayerState<AEmbercorePlayerState>();
	if (!PS) {
		return;
	}

	UIHUDWidget = CreateWidget<UEmbercoreHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / FMath::Max<float>(PS->GetMaxHealth(), 1.f));
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / FMath::Max<float>(PS->GetMaxMana(), 1.f));
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / FMath::Max<float>(PS->GetMaxStamina(), 1.f));
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetExperience(PS->GetXP());
	UIHUDWidget->SetGold(PS->GetGold());
	UIHUDWidget->SetLevel(PS->GetCharacterLevel());

	DamageNumberClass = StaticLoadClass(UObject::StaticClass(), nullptr,
	                                    TEXT("/Game/UI/WC_DamageText.WC_DamageText_C"));
	if (!DamageNumberClass) {
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "%s() Failed to find DamageNumberClass. If it was moved, please update the reference location in C++."
		       ), *FString(__FUNCTION__));
	}
}

void AEmbercorePlayerController::BeginPlay() {
	Super::BeginPlay();

	this->EnableInput(this);
}

UEmbercoreHUDWidget* AEmbercorePlayerController::GetHUD() const {
	return UIHUDWidget;
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

void AEmbercorePlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining) {
	if (UIHUDWidget) {
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

bool AEmbercorePlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining) {
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

void AEmbercorePlayerController::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}
