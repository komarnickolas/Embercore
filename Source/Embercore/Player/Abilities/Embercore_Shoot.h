// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Embercore/Abilities/EmbercoreGameplayAbility.h"
#include "Embercore/Weapons/EmbercoreProjectile.h"
#include "Embercore_Shoot.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API UEmbercore_Shoot : public UEmbercoreGameplayAbility {
	GENERATED_BODY()
public:
	UEmbercore_Shoot();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AEmbercoreProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

};
