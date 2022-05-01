// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EmbercoreAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UEmbercoreAbilitySystemComponent*, SourceASC,
                                               float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * 
 */
UCLASS()
class EMBERCORE_API UEmbercoreAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()
public:
	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;

	// Called from GDDamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UEmbercoreAbilitySystemComponent* SourceASC, float UnmitigatedDamage,
	                           float MitigatedDamage);
};
