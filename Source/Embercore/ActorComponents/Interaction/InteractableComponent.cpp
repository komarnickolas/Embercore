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
}

void UInteractableComponent::ToggleInput(APlayerController* Controller, bool InToggle) {
	OnToggleInputEvent.Broadcast(InToggle);
	ToggleOwnerInput(Controller, InToggle);
	BindToInput(InToggle);
	SetHiddenInGame(!InToggle, true);
}

void UInteractableComponent::ToggleOwnerInput(APlayerController* Controller, bool Enable) {
	Enable ? GetOwner()->EnableInput(Controller) : GetOwner()->DisableInput(Controller);
}

void UInteractableComponent::BindToInput(bool Enable) {
	if (!IsValid(GetOwner()->InputComponent)) { return; }
	if (Enable) {
		GetOwner()->InputComponent->BindAction("Interact", IE_Pressed, this,
		                                       &UInteractableComponent::OnInteractPressed);
		GetOwner()->InputComponent->BindAction("Interact", IE_Released, this,
		                                       &UInteractableComponent::OnInteractReleased);
	}
	else {
		GetOwner()->InputComponent->RemoveActionBinding("Interact", IE_Pressed);
		GetOwner()->InputComponent->RemoveActionBinding("Interact", IE_Released);
	}
}

// Called when the game starts
void UInteractableComponent::BeginPlay() {
	Super::BeginPlay();
}

void UInteractableComponent::StartTimer() {
	GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
	GetWorld()->GetTimerManager().SetTimer(InteractTimer, this, &UInteractableComponent::UpdateTimer, MaxHoldTime);
	OnStartTimerEvent.Broadcast();
}

void UInteractableComponent::StopTimer() {
	GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
	GetWorld()->GetTimerManager().ClearTimer(DelayInteractionTimerHandle);
	UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetWidget());
	if (IsValid(InteractionWidget)) { InteractionWidget->Reset(); }
	OnStopTimerEvent.Broadcast();
}

void UInteractableComponent::UpdateTimer() {
	if (this->IsHeld) {
		float TimeHeld = GetWorld()->GetTimerManager().GetTimerElapsed(InteractTimer);
		Cast<UInteractionWidget>(GetWidget())->SetPercent(TimeHeld, MaxHoldTime);
		GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UInteractableComponent::UpdateTimer, 0.1f);
	}
	else {
		StopTimer();
	}
	OnUpdateTimerEvent.Broadcast(this->IsHeld);
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
	this->IsHeld = false;
	StopTimer();
	OnInteractEvent.Broadcast();
}

void UInteractableComponent::OnInteractReleased() {
	this->IsHeld = false;
	StopTimer();
}


// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
