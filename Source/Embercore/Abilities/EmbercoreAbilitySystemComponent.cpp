// Fill out your copyright notice in the Description page of Project Settings.


#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"

void UEmbercoreAbilitySystemComponent::ReceiveDamage(UEmbercoreAbilitySystemComponent* SourceASC,
                                                     float UnmitigatedDamage, float MitigatedDamage) {
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
