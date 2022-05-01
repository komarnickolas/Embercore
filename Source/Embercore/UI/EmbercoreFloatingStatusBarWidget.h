// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmbercoreFloatingStatusBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API UEmbercoreFloatingStatusBarWidget : public UUserWidget {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaPercentage(float ManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCharacterName(const FText& NewName);
};
