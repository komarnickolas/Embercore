// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API UInteractionWidget : public UUserWidget {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetPercent(float InTimeHeld, float InMaxTimeHeld);
	UFUNCTION(BlueprintCallable)
	void Reset();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float TimePercent = 0.0f;
};
