// Fill out your copyright notice in the Description page of Project Settings.


#include "Embercore/Player/EmbercorePlayer.h"

#include "EmbercorePlayerController.h"
#include "EmbercorePlayerHUD.h"
#include "EmbercorePlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Embercore/EmbercoreGameMode.h"
#include "Embercore/EmbercoreAttributeSet.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "Embercore/Inventory/InventoryComponent.h"
#include "Embercore/UI/EmbercoreFloatingStatusBarWidget.h"
#include "Embercore/Weapons/EmbercoreWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEmbercorePlayer::AEmbercorePlayer(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(50, 2));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AEmbercorePlayer::PreviousWeapon() {
	InventoryComponent->SwitchWeapon(-1);
}

void AEmbercorePlayer::NextWeapon() {
	InventoryComponent->SwitchWeapon(1);
}

void AEmbercorePlayer::SelectWeapon(int32 Index) {
	InventoryComponent->SelectWeapon(Index);
}

// Called to bind functionality to input
void AEmbercorePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AEmbercorePlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEmbercorePlayer::MoveRight);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AEmbercorePlayer::NextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AEmbercorePlayer::PreviousWeapon);

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}


// Server only
void AEmbercorePlayer::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	AEmbercorePlayerState* PS = GetPlayerState<AEmbercorePlayerState>();
	if (PS) {
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UEmbercoreAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());

		// End respawn specific things


		AddStartupEffects();

		AddCharacterAbilities();

		InitializeFloatingStatusBar();
	}
}

AEmbercorePlayerHUD* AEmbercorePlayer::GetHUD() {
	return Cast<AEmbercorePlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

USpringArmComponent* AEmbercorePlayer::GetCameraBoom() {
	return CameraBoom;
}

UCameraComponent* AEmbercorePlayer::GetFollowCamera() {
	return FollowCamera;
}

float AEmbercorePlayer::GetStartingCameraBoomArmLength() {
	return StartingCameraBoomArmLength;
}

FVector AEmbercorePlayer::GetStartingCameraBoomLocation() {
	return StartingCameraBoomLocation;
}

UEmbercoreFloatingStatusBarWidget* AEmbercorePlayer::GetFloatingStatusBar() {
	return UIFloatingStatusBar;
}

void AEmbercorePlayer::FinishDying() {
	if (GetLocalRole() == ROLE_Authority) {
		if (AEmbercoreGameMode* GM = Cast<AEmbercoreGameMode>(GetWorld()->GetAuthGameMode())) {
			GM->PlayerDied();
		}
	}

	Super::FinishDying();
}

/**
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void AEmbercorePlayer::BeginPlay() {
	Super::BeginPlay();
	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	InitializeFloatingStatusBar();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();
	InventoryComponent->AddWeapon(StartingWeapon);
}

void AEmbercorePlayer::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AEmbercorePlayer::LookUp(float Value) {
	if (IsAlive()) {
		AddControllerPitchInput(Value);
	}
}

void AEmbercorePlayer::LookUpRate(float Value) {
	if (IsAlive()) {
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
	}
}

void AEmbercorePlayer::Turn(float Value) {
	if (IsAlive()) {
		AddControllerYawInput(Value);
	}
}

void AEmbercorePlayer::TurnRate(float Value) {
	if (IsAlive()) {
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
	}
}

void AEmbercorePlayer::MoveForward(float Value) {
	AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void AEmbercorePlayer::MoveRight(float Value) {
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void AEmbercorePlayer::InitializeFloatingStatusBar() {
	// Only create once
	if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid()) {
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AEmbercorePlayerController* PC = Cast<AEmbercorePlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController()) {
		if (UIFloatingStatusBarClass) {
			UIFloatingStatusBar = CreateWidget<UEmbercoreFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent) {
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
				UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
			}
		}
	}
}

// Client only
void AEmbercorePlayer::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	AEmbercorePlayerState* PS = GetPlayerState<AEmbercorePlayerState>();
	if (PS) {
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UEmbercoreAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		BindASCInput();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}

void AEmbercorePlayer::BindASCInput() {
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent)) {
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(
			                                                              FString("ConfirmTarget"),
			                                                              FString("CancelTarget"),
			                                                              FString("EEmbercoreAbilityInputID"),
			                                                              static_cast<int32>(
				                                                              EEmbercoreAbilityInputID::Confirm),
			                                                              static_cast<int32>(
				                                                              EEmbercoreAbilityInputID::Cancel)));

		ASCInputBound = true;
	}
}
