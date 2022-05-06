// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Embercore/EmbercoreGameMode.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	InventoryName = "";
	Capacity = 0;
}


void UInventoryComponent::AddWeapon(FName WeaponID) {
	for (int i = 0; i < WeaponInventory.Num(); i++) {
		if (WeaponInventory[i].WeaponId == WeaponID) {
			if (!bHasEquippedWeapon) {
				IndexEquippedWeapon = i;
				WeaponIDEquipped = WeaponID;
				bHasEquippedWeapon = true;
				WeaponInventory[i].SpawnedItem->SetActorHiddenInGame(false);
			}
			OnRefreshWeaponInventory();
			return;
		}
	}

	AEmbercoreGameMode* GameMode = Cast<AEmbercoreGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr) {
		bool Found = false;
		FWeaponDataStructure WeaponFound = GameMode->FindWeapon(WeaponID, Found);
		if (Found && (WeaponFound.WeaponActor != nullptr)) {
		}
	}
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
