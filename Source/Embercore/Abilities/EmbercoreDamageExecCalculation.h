// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "EmbercoreDamageExecCalculation.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API UEmbercoreDamageExecCalculation : public UGameplayEffectExecutionCalculation {
	GENERATED_BODY()
public:
	UEmbercoreDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
