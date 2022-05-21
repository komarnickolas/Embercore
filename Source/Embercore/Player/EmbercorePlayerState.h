// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "EmbercorePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API AEmbercorePlayerState : public APlayerState, public IAbilitySystemInterface {
	GENERATED_BODY()
public:
	AEmbercorePlayerState();
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UEmbercoreAttributeSet* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState")
	bool IsAlive() const;

	// UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|UI")
	// void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	int32 GetXPBounty() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "EmbercorePlayerState|Attributes")
	int32 GetGoldBounty() const;

protected:
	UPROPERTY()
	class UEmbercoreAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UEmbercoreAttributeSet* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle XPChangedDelegateHandle;
	FDelegateHandle GoldChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void XPChanged(const FOnAttributeChangeData& Data);
	virtual void GoldChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
