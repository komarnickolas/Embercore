// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercoreAT_WaitReceiveDamage.h"

#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"

UEmbercoreAT_WaitReceiveDamage::UEmbercoreAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	TriggerOnce = false;
}

UEmbercoreAT_WaitReceiveDamage* UEmbercoreAT_WaitReceiveDamage::WaitReceiveDamage(
	UGameplayAbility* OwningAbility, bool InTriggerOnce) {
	UEmbercoreAT_WaitReceiveDamage* MyObj = NewAbilityTask<UEmbercoreAT_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UEmbercoreAT_WaitReceiveDamage::Activate() {
	UEmbercoreAbilitySystemComponent* EmbercoreASC = Cast<UEmbercoreAbilitySystemComponent>(AbilitySystemComponent);

	if (EmbercoreASC) {
		EmbercoreASC->ReceivedDamage.AddDynamic(this, &UEmbercoreAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UEmbercoreAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding) {
	UEmbercoreAbilitySystemComponent* EmbercoreASC = Cast<UEmbercoreAbilitySystemComponent>(AbilitySystemComponent);

	if (EmbercoreASC) {
		EmbercoreASC->ReceivedDamage.RemoveDynamic(this, &UEmbercoreAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UEmbercoreAT_WaitReceiveDamage::OnDamageReceived(UEmbercoreAbilitySystemComponent* SourceASC,
                                                      float UnmitigatedDamage, float MitigatedDamage) {
	if (ShouldBroadcastAbilityTaskDelegates()) {
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (TriggerOnce) {
		EndTask();
	}
}
