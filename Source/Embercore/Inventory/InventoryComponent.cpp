// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InventoryName = "";
	Capacity = 0;
}

// Called when the game starts
void UInventoryComponent::BeginPlay() {
	Super::BeginPlay();

	// ...

}

bool UInventoryComponent::AddItem(FString Name, FString Description, TSubclassOf<AActor> Actor) {
	FItemStructure Item;
	Item.Name = Name;
	Item.Description = Description;
	Item.Actor = Actor;
	if (Inventory.Num() >= Capacity) { return false; }
	Inventory.Add(Item);
	ItemAddedEvent.Broadcast(Item);
	return true;
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
