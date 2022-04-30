// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Embercore/Abilities/AsyncTask/AsyncTaskAttributeChanged.h"
#include "Minion.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API AMinion : public AEmbercoreCharacter {
	GENERATED_BODY()

public:
	AMinion(const class FObjectInitializer& ObjectInitializer);

protected:
	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	class UEmbercoreAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	class UEmbercoreAttributeSet* HardRefAttributeSetBase;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UEmbercoreFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Embercore|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
