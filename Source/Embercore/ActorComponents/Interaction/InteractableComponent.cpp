// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "InteractionWidget.h"
#include "Components/BoxComponent.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bHiddenInGame = true;

	// ...
}

void UInteractableComponent::ToggleInput(APlayerController* Controller, bool InToggle) {
	this->Active = InToggle;
	if (Active) {
		BindToInput("Interact", true);
		GetOwner()->EnableInput(Controller);
		SetHiddenInGame(false, true);
	}
	else {
		BindToInput("Interact", false);
		GetOwner()->DisableInput(Controller);
		SetHiddenInGame(true, true);
	}
	StopTimer();
}

void UInteractableComponent::BindToInput(FName ActionName, bool Enable) {
	if (!IsValid(GetOwner()->InputComponent)) { return; }
	if (Enable) {
		GetOwner()->InputComponent->BindAction(ActionName, IE_Pressed, this,
		                                       &UInteractableComponent::OnInteractPressed);
		GetOwner()->InputComponent->BindAction(ActionName, IE_Released, this,
		                                       &UInteractableComponent::OnInteractReleased);
	}
	else {
		GetOwner()->InputComponent->RemoveActionBinding(ActionName, IE_Pressed);
		GetOwner()->InputComponent->RemoveActionBinding(ActionName, IE_Released);
	}
}

// Called when the game starts
void UInteractableComponent::BeginPlay() {
	Super::BeginPlay();
}

void UInteractableComponent::StartTimer() {
	GetWorld()->GetTimerManager().SetTimer(InteractTimer, this, &UInteractableComponent::UpdateTimer, MaxHoldTime);
}

void UInteractableComponent::StopTimer() {
	GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
	UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetWidget());
	if (IsValid(InteractionWidget)) {
		InteractionWidget->Reset();
	}
}

void UInteractableComponent::UpdateTimer() {
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	if (IsHeld) {
		float TimeHeld = GetWorld()->GetTimerManager().GetTimerElapsed(InteractTimer);
		Cast<UInteractionWidget>(GetWidget())->SetPercent(TimeHeld, MaxHoldTime);
		GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UInteractableComponent::UpdateTimer, 0.1f);
	}
	else {
		StopTimer();
	}
}

void UInteractableComponent::OnInteractPressed() {
	this->IsHeld = true;
	DelayInteraction();
}

void UInteractableComponent::DelayInteraction() {
	GetWorld()->GetTimerManager().ClearTimer(DelayInteractionTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(DelayInteractionTimerHandle, this, &UInteractableComponent::TriggerInteract,
	                                       MaxHoldTime);
	StartTimer();
	UpdateTimer();
}

void UInteractableComponent::TriggerInteract() {
	OnInteractEvent.Broadcast();
}

void UInteractableComponent::OnInteractReleased() {
	this->IsHeld = false;
	GetWorld()->GetTimerManager().ClearTimer(DelayInteractionTimerHandle);
	StopTimer();
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
