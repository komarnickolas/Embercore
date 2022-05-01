// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EmbercoreDamageTextWidgetComponent.generated.h"
/**
 * 
 */
UCLASS()
class EMBERCORE_API UEmbercoreDamageTextWidgetComponent : public UWidgetComponent {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage);
};
