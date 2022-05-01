// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercoreCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EmbercoreCharacter.h"
#include "GameplayTagContainer.h"

UEmbercoreCharacterMovementComponent::UEmbercoreCharacterMovementComponent() {
	SprintSpeedMultiplier = 1.4f;
	ADSSpeedMultiplier = 0.5f;
}

float UEmbercoreCharacterMovementComponent::GetMaxSpeed() const {
	AEmbercoreCharacter* Owner = Cast<AEmbercoreCharacter>(GetOwner());
	if (!Owner) {
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive()) {
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")))) {
		return 0.0f;
	}

	if (RequestToStartSprinting) {
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (RequestToStartADS) {
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}

void UEmbercoreCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags) {
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UEmbercoreCharacterMovementComponent::GetPredictionData_Client() const {
	check(PawnOwner != NULL);

	if (!ClientPredictionData) {
		UEmbercoreCharacterMovementComponent* MutableThis = const_cast<UEmbercoreCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FGDNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UEmbercoreCharacterMovementComponent::StartSprinting() {
	RequestToStartSprinting = true;
}

void UEmbercoreCharacterMovementComponent::StopSprinting() {
	RequestToStartSprinting = false;
}

void UEmbercoreCharacterMovementComponent::StartAimDownSights() {
	RequestToStartADS = true;
}

void UEmbercoreCharacterMovementComponent::StopAimDownSights() {
	RequestToStartADS = false;
}

void UEmbercoreCharacterMovementComponent::FGDSavedMove::Clear() {
	Super::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartADS = false;
}

uint8 UEmbercoreCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const {
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting) {
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS) {
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool UEmbercoreCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                        ACharacter* Character, float MaxDelta) const {
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting) {
		return false;
	}

	if (SavedRequestToStartADS != ((FGDSavedMove*)&NewMove)->SavedRequestToStartADS) {
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UEmbercoreCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime,
                                                                    FVector const& NewAccel,
                                                                    FNetworkPredictionData_Client_Character&
                                                                    ClientData) {
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UEmbercoreCharacterMovementComponent* CharacterMovement = Cast<UEmbercoreCharacterMovementComponent>(
		Character->GetCharacterMovement());
	if (CharacterMovement) {
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartADS = CharacterMovement->RequestToStartADS;
	}
}

void UEmbercoreCharacterMovementComponent::FGDSavedMove::PrepMoveFor(ACharacter* Character) {
	Super::PrepMoveFor(Character);

	UEmbercoreCharacterMovementComponent* CharacterMovement = Cast<UEmbercoreCharacterMovementComponent>(
		Character->GetCharacterMovement());
	if (CharacterMovement) {
	}
}

UEmbercoreCharacterMovementComponent::FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(
	const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement) {
}

FSavedMovePtr UEmbercoreCharacterMovementComponent::FGDNetworkPredictionData_Client::AllocateNewMove() {
	return FSavedMovePtr(new FGDSavedMove());
}
