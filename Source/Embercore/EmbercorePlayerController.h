// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmbercoreCharacter.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "EmbercorePlayerController.generated.h"

UCLASS()
class EMBERCORE_API AEmbercorePlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AEmbercorePlayerController();
	void CreateHUD();

	UPROPERTY(EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreDamageTextWidgetComponent> DamageNumberClass;

	class UEmbercoreHUDWidget* GetHUD();

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AEmbercoreCharacter* TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AEmbercoreCharacter* TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AEmbercoreCharacter* TargetCharacter);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Embercore|UI")
	class UEmbercoreHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};
