// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "EmbercorePlayerController.generated.h"

UCLASS()
class EMBERCORE_API AEmbercorePlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AEmbercorePlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreDamageTextWidgetComponent> DamageNumberClass;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AEmbercoreCharacter* TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AEmbercoreCharacter* TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AEmbercoreCharacter* TargetCharacter);
protected:
	// Server only
	virtual void OnPossess(APawn* InPawn) override;
};
