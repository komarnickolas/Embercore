// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercorePlayerState.h"

#include "EmbercorePlayerController.h"
#include "EmbercorePlayerHUD.h"
#include "EmbercorePlayer.h"
#include "Embercore/EmbercoreAttributeSet.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "Embercore/UI/EmbercoreFloatingStatusBarWidget.h"
#include "Embercore/UI/EmbercoreHUDWidget.h"

AEmbercorePlayerState::AEmbercorePlayerState() {
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UEmbercoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Player) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UEmbercoreAttributeSet>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AEmbercorePlayerState::GetAbilitySystemComponent() const {
	return AbilitySystemComponent;
}

UEmbercoreAttributeSet* AEmbercorePlayerState::GetAttributeSetBase() const {
	return AttributeSetBase;
}

bool AEmbercorePlayerState::IsAlive() const {
	return GetHealth() > 0.0f;
}

float AEmbercorePlayerState::GetHealth() const {
	return AttributeSetBase->GetHealth();
}

float AEmbercorePlayerState::GetMaxHealth() const {
	return AttributeSetBase->GetMaxHealth();
}

float AEmbercorePlayerState::GetHealthRegenRate() const {
	return AttributeSetBase->GetHealthRegenRate();
}

float AEmbercorePlayerState::GetArmor() const {
	return AttributeSetBase->GetArmor();
}

float AEmbercorePlayerState::GetMoveSpeed() const {
	return AttributeSetBase->GetMoveSpeed();
}

int32 AEmbercorePlayerState::GetCharacterLevel() const {
	return AttributeSetBase->GetCharacterLevel();
}

int32 AEmbercorePlayerState::GetXP() const {
	return AttributeSetBase->GetXP();
}

int32 AEmbercorePlayerState::GetXPBounty() const {
	return AttributeSetBase->GetXPBounty();
}

int32 AEmbercorePlayerState::GetGold() const {
	return AttributeSetBase->GetGold();
}

int32 AEmbercorePlayerState::GetGoldBounty() const {
	return AttributeSetBase->GetGoldBounty();
}

void AEmbercorePlayerState::BeginPlay() {
	Super::BeginPlay();

	if (AbilitySystemComponent) {
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->
		                              GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).
		                              AddUObject(this, &AEmbercorePlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->
		                                 GetGameplayAttributeValueChangeDelegate(
			                                 AttributeSetBase->GetMaxHealthAttribute()).AddUObject(
			                                 this, &AEmbercorePlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->
		                                       GetGameplayAttributeValueChangeDelegate(
			                                       AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(
			                                       this, &AEmbercorePlayerState::HealthRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->
		                          GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).
		                          AddUObject(this, &AEmbercorePlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->
		                            GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).
		                            AddUObject(this, &AEmbercorePlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->
		                                      GetGameplayAttributeValueChangeDelegate(
			                                      AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(
			                                      this, &AEmbercorePlayerState::CharacterLevelChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AEmbercorePlayerState::StunTagChanged);
	}
}


void AEmbercorePlayerState::HealthChanged(const FOnAttributeChangeData& Data) {
	float Health = Data.NewValue;
	AEmbercorePlayer* Player = Cast<AEmbercorePlayer>(GetPawn());
	if (Player) {
		UEmbercoreFloatingStatusBarWidget* PlayerFloatingStatusBar = Player->GetFloatingStatusBar();
		if (PlayerFloatingStatusBar) {
			PlayerFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}
	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag)) {
		if (Player) {
			Player->Die();
		}
	}
}

void AEmbercorePlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data) {
	float MaxHealth = Data.NewValue;
	AEmbercorePlayer* Player = Cast<AEmbercorePlayer>(GetPawn());
	if (Player) {
		UEmbercoreFloatingStatusBarWidget* PlayerFloatingStatusBar = Player->GetFloatingStatusBar();
		if (PlayerFloatingStatusBar) {
			PlayerFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}
}

void AEmbercorePlayerState::HealthRegenRateChanged(const FOnAttributeChangeData& Data) {
	float HealthRegenRate = Data.NewValue;
}

void AEmbercorePlayerState::XPChanged(const FOnAttributeChangeData& Data) {
	float XP = Data.NewValue;
}

void AEmbercorePlayerState::GoldChanged(const FOnAttributeChangeData& Data) {
	float Gold = Data.NewValue;
}

void AEmbercorePlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data) {
	float CharacterLevel = Data.NewValue;
}

void AEmbercorePlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) {
	if (NewCount > 0) {
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
