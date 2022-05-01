// Fill out your copyright notice in the Description page of Project Settings.


#include "Minion.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Embercore/EmbercoreAttributeSet.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "Embercore/UI/EmbercoreFloatingStatusBarWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AMinion::AMinion(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Create ability system component, and set it to be explicitly replicated
	HardRefAbilitySystemComponent = CreateDefaultSubobject<UEmbercoreAbilitySystemComponent>(
		TEXT("AbilitySystemComponent"));
	HardRefAbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	HardRefAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Set our parent's TWeakObjectPtr
	AbilitySystemComponent = HardRefAbilitySystemComponent;

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	HardRefAttributeSetBase = CreateDefaultSubobject<UEmbercoreAttributeSet>(TEXT("AttributeSetBase"));

	// Set our parent's TWeakObjectPtr
	AttributeSetBase = HardRefAttributeSetBase;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(50, 2));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr,
	                                           TEXT(
		                                           "/Game/UI/UI_FloatingStatusBar_Minion.UI_FloatingStatusBar_Minion_C"));
	if (!UIFloatingStatusBarClass) {
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."
		       ), *FString(__FUNCTION__));
	}
}

void AMinion::BeginPlay() {
	Super::BeginPlay();

	if (AbilitySystemComponent.IsValid()) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();

		// Setup FloatingStatusBar UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->IsLocalPlayerController()) {
			if (UIFloatingStatusBarClass) {
				UIFloatingStatusBar = CreateWidget<UEmbercoreFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
				if (UIFloatingStatusBar && UIFloatingStatusBarComponent) {
					UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

					// Setup the floating status bar
					UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());

					UIFloatingStatusBar->SetCharacterName(CharacterName);
				}
			}
		}

		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AMinion::HealthChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AMinion::StunTagChanged);
	}
}

void AMinion::HealthChanged(const FOnAttributeChangeData& Data) {
	float Health = Data.NewValue;

	// Update floating status bar
	if (UIFloatingStatusBar) {
		UIFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
	}

	// If the minion died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag)) {
		Die();
	}
}

void AMinion::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) {
	if (NewCount > 0) {
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
