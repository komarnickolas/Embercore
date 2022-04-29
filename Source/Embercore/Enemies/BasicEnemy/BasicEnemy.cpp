// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemy.h"

#include "BasicEnemyAIController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Embercore/EmbercoreAttributeSet.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "Embercore/Player/PlayerCharacter.h"
#include "Embercore/UI/EmbercoreFloatingStatusBarWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasicEnemy::ABasicEnemy(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	PlayerCollisionDetection->SetupAttachment(RootComponent);

	PlayerAttackCollisionDetection = CreateDefaultSubobject<
		USphereComponent>(TEXT("Player Attack Collision Detection"));
	PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(GetMesh(), TEXT("RootSocket"));

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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(50, 2));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr,
	                                           TEXT(
		                                           "/Game/UI/UI_FloatingStatusBar_BasicEnemy.UI_FloatingStatusBar_BasicEnemy_C"));
	if (!UIFloatingStatusBarClass) {
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."
		       ), *FString(__FUNCTION__));
	}
}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay() {
	Super::BeginPlay();

	BasicEnemyAIController = Cast<ABasicEnemyAIController>(GetController());

	PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerDetectedOverlapBegin);
	PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerDetectedOverlapEnd);

	PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerAttackOverlapBegin);
	PlayerAttackCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerAttackOverlapEnd);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnDealDamageOverlapBegin);

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
		HealthChangedDelegateHandle = AbilitySystemComponent->
		                              GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).
		                              AddUObject(this, &ABasicEnemy::HealthChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &ABasicEnemy::StunTagChanged);
	}
}


void ABasicEnemy::HealthChanged(const FOnAttributeChangeData& Data) {
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

void ABasicEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) {
	if (NewCount > 0) {
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}

// Called every frame
void ABasicEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ABasicEnemy::SeekPlayer() {
	if (PlayerRef) {
		UE_LOG(LogTemp, Warning, TEXT("Seeking Player"));
		BasicEnemyAIController->MoveToLocation(PlayerRef->GetActorLocation(), StoppingDistance, true);
		GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimeHandle, this, &ABasicEnemy::SeekPlayer, 0.25f, true);
	}
}

void ABasicEnemy::StopSeekingPlayer() {
	GetWorld()->GetTimerManager().ClearTimer(SeekPlayerTimeHandle);
}

void ABasicEnemy::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult) {
	PlayerRef = Cast<APlayerCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = true;
	}
}

void ABasicEnemy::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	PlayerRef = Cast<APlayerCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = false;
	}
}

void ABasicEnemy::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult) {
	PlayerRef = Cast<APlayerCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = true;
		CanAttackPlayer = true;
	}
}

void ABasicEnemy::OnPlayerAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	PlayerRef = Cast<APlayerCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = false;
		CanAttackPlayer = false;
	}
}

void ABasicEnemy::OnDealDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult) {
	PlayerRef = Cast<APlayerCharacter>(OtherActor);
	if (PlayerRef && CanDealDamage) {
		UE_LOG(LogTemp, Warning, TEXT("Player Damaged"));
	}
}
