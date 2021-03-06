// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercoreAttributeSet.h"

#include "Embercore.h"
#include "EmbercoreCharacter.h"
#include "Player/EmbercorePlayerController.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UEmbercoreAttributeSet::UEmbercoreAttributeSet() {
	// Cache tags
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
}

void UEmbercoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute())
	// GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute()) {
		// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
}

void UEmbercoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AEmbercoreCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid()) {
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AEmbercoreCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AEmbercoreCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid()) {
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr) {
			if (APawn* Pawn = Cast<APawn>(SourceActor)) {
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController) {
			SourceCharacter = Cast<AEmbercoreCharacter>(SourceController->GetPawn());
		}
		else {
			SourceCharacter = Cast<AEmbercoreCharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser()) {
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) {
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.0f) {
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter) {
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive()) {
				//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			// Apply the health change and then clamp it
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			if (TargetCharacter && WasAlive) {
				// Show damage number for the Source player unless it was self damage
				if (SourceActor != TargetActor) {
					AEmbercorePlayerController* PC = Cast<AEmbercorePlayerController>(SourceController);
					if (PC) {
						PC->ShowDamageNumber(LocalDamageDone, TargetCharacter);
					}
				}

				if (!TargetCharacter->IsAlive()) {
					// TargetCharacter was alive before this damage and now is not alive, give XP and Gold bounties to Source.
					// Don't give bounty to self.
					if (SourceController != TargetController) {
						// Create a dynamic instant Gameplay Effect to give the bounties
						UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(
							GetTransientPackage(), FName(TEXT("Bounty")));
						GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

						int32 Idx = GEBounty->Modifiers.Num();
						GEBounty->Modifiers.SetNum(Idx + 2);

						FGameplayModifierInfo& InfoXP = GEBounty->Modifiers[Idx];
						InfoXP.ModifierMagnitude = FScalableFloat(GetXPBounty());
						InfoXP.ModifierOp = EGameplayModOp::Additive;
						InfoXP.Attribute = UEmbercoreAttributeSet::GetXPAttribute();

						FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
						InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
						InfoGold.ModifierOp = EGameplayModOp::Additive;
						InfoGold.Attribute = UEmbercoreAttributeSet::GetGoldAttribute();

						Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
					}
				}
			}
		}
	} // Damage
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
}

void UEmbercoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, XP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, XPBounty, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEmbercoreAttributeSet, GoldBounty, COND_None, REPNOTIFY_Always);
}

void UEmbercoreAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
                                                         const FGameplayAttributeData& MaxAttribute,
                                                         float NewMaxValue,
                                                         const FGameplayAttribute& AffectedAttributeProperty) {
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp) {
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f)
			                 ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			                 : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UEmbercoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, Health, OldHealth);
}

void UEmbercoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, MaxHealth, OldMaxHealth);
}

void UEmbercoreAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UEmbercoreAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, Armor, OldArmor);
}

void UEmbercoreAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UEmbercoreAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, CharacterLevel, OldCharacterLevel);
}

void UEmbercoreAttributeSet::OnRep_XP(const FGameplayAttributeData& OldXP) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, XP, OldXP);
}

void UEmbercoreAttributeSet::OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, XPBounty, OldXPBounty);
}

void UEmbercoreAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, Gold, OldGold);
}

void UEmbercoreAttributeSet::OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty) {
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEmbercoreAttributeSet, GoldBounty, OldGoldBounty);
}
