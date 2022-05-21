// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercoreCharacter.h"

#include "EmbercoreCharacterMovementComponent.h"
#include "EmbercoreAttributeSet.h"
#include "Abilities/EmbercoreGameplayAbility.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AEmbercoreCharacter::AEmbercoreCharacter(const class FObjectInitializer& ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,
	                                                     ECR_Overlap);

	bAlwaysRelevant = true;

	// Cache tags
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(FName("InventoryComponent"));
	InventoryComponent->InventoryName = "Backpack";
	InventoryComponent->Capacity = 3;
	InventoryComponent->WeaponCapacity = 3;
}

void AEmbercoreCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

UAbilitySystemComponent* AEmbercoreCharacter::GetAbilitySystemComponent() const {
	return AbilitySystemComponent.Get();
}

UInventoryComponent* AEmbercoreCharacter::GetInventory() {
	return InventoryComponent;
}

bool AEmbercoreCharacter::IsAlive() const {
	return GetHealth() > 0.0f;
}

int32 AEmbercoreCharacter::GetAbilityLevel(EEmbercoreAbilityInputID AbilityID) const {
	return 1;
}

void AEmbercoreCharacter::RemoveCharacterAbilities() {
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->
		CharacterAbilitiesGiven) {
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities()) {
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass())) {
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++) {
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

EEmbercoreHitReactDirection AEmbercoreCharacter::GetHitReactDirection(const FVector& ImpactPoint) {
	const FVector& ActorLocation = GetActorLocation();
	// PointPlaneDist is super cheap - 1 vector subtraction, 1 dot product.
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());


	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane)) {
		// Determine if Front or Back

		// Can see if it's left or right of Left/Right plane which would determine Front or Back
		if (DistanceToRightLeftPlane >= 0) {
			return EEmbercoreHitReactDirection::Front;
		}
		return EEmbercoreHitReactDirection::Back;
	}
	// Determine if Right or Left

	if (DistanceToFrontBackPlane >= 0) {
		return EEmbercoreHitReactDirection::Right;
	}
	return EEmbercoreHitReactDirection::Left;

	return EEmbercoreHitReactDirection::Front;
}

void AEmbercoreCharacter::PlayHitReact_Implementation(FGameplayTag HitDirection, AActor* DamageCauser) {
	if (IsAlive()) {
		if (HitDirection == HitDirectionLeftTag) {
			ShowHitReact.Broadcast(EEmbercoreHitReactDirection::Left);
		}
		else if (HitDirection == HitDirectionFrontTag) {
			ShowHitReact.Broadcast(EEmbercoreHitReactDirection::Front);
		}
		else if (HitDirection == HitDirectionRightTag) {
			ShowHitReact.Broadcast(EEmbercoreHitReactDirection::Right);
		}
		else if (HitDirection == HitDirectionBackTag) {
			ShowHitReact.Broadcast(EEmbercoreHitReactDirection::Back);
		}
	}
}

bool AEmbercoreCharacter::PlayHitReact_Validate(FGameplayTag HitDirection, AActor* DamageCauser) {
	return true;
}

int32 AEmbercoreCharacter::GetCharacterLevel() const {
	if (AttributeSetBase.IsValid()) {
		return static_cast<int32>(AttributeSetBase->GetCharacterLevel());
	}

	return 0;
}

float AEmbercoreCharacter::GetHealth() const {
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AEmbercoreCharacter::GetMaxHealth() const {
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float AEmbercoreCharacter::GetMoveSpeed() const {
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float AEmbercoreCharacter::GetMoveSpeedBaseValue() const {
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->
		                         GetBaseValue();
	}

	return 0.0f;
}

// Run on Server and all clients
void AEmbercoreCharacter::Die() {
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid()) {
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage) {
		PlayAnimMontage(DeathMontage);
	}
	else {
		FinishDying();
	}
}

void AEmbercoreCharacter::FinishDying() {
	Destroy();
}

// Called when the game starts or when spawned
void AEmbercoreCharacter::BeginPlay() {
	Super::BeginPlay();
}

void AEmbercoreCharacter::AddCharacterAbilities() {
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->
		CharacterAbilitiesGiven) {
		return;
	}

	for (TSubclassOf<UEmbercoreGameplayAbility>& StartupAbility : CharacterAbilities) {
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID),
			                     static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void AEmbercoreCharacter::InitializeAttributes() {
	if (!AbilitySystemComponent.IsValid()) {
		return;
	}

	if (!DefaultAttributes) {
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."),
		       *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid()) {
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
			*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AEmbercoreCharacter::AddStartupEffects() {
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->
		StartupEffectsApplied) {
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects) {
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid()) {
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
				*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}

void AEmbercoreCharacter::SetHealth(float Health) {
	if (AttributeSetBase.IsValid()) {
		AttributeSetBase->SetHealth(Health);
	}
}
