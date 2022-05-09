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
			}
			OnRefreshWeaponInventory.Broadcast();
			return;
		}
	}

	AEmbercoreGameMode* GameMode = Cast<AEmbercoreGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr) {
		bool Found = false;
		const FWeaponDataStructure WeaponFound = GameMode->FindWeapon(WeaponID, Found);
		if (Found && (WeaponFound.WeaponActor != nullptr)) {
			WeaponInventory.Add(WeaponFound);
			OnRefreshWeaponInventory.Broadcast();
		}
	}
	OnRefreshWeaponInventory.Broadcast();
}

int32 UInventoryComponent::FindWeaponInventoryIndex(FName WeaponID) {
	for (int i = 0; i < WeaponInventory.Num(); i++) {
		if (WeaponInventory[i].WeaponId == WeaponID) {
			return i;
		}
	}
	return -1;
}

void UInventoryComponent::RemoveWeapon(FName WeaponID, bool RemoveEquippedWeapon) {
	int32 IndexToRemove = FindWeaponInventoryIndex(WeaponID);
	if (IndexToRemove >= 0) {
		WeaponInventory.RemoveAt(IndexToRemove);
	}
	OnRefreshWeaponInventory.Broadcast();
}

bool UInventoryComponent::HasFreeWeaponSlot() {
	return WeaponInventory.Num() < WeaponCapacity;
}

bool UInventoryComponent::HasEquippedWeapon(FName WeaponID) {
	if (bHasEquippedWeapon) {
		return WeaponID == WeaponIDEquipped;
	}
	return false;
}

TSubclassOf<AEmbercoreWeapon> UInventoryComponent::GetCurrentlyEquippedWeapon() {
	if (WeaponInventory.Num() > 0) {
		return WeaponInventory[IndexEquippedWeapon].WeaponActor;
	}
	return {};
}

void UInventoryComponent::SelectWeapon(int32 Index) {
	if (!bHasEquippedWeapon) { return; }
	if (WeaponInventory.Num() == 0) { return; }
	if (Index < 0 || Index > WeaponInventory.Num()) { return; }
	IndexEquippedWeapon = Index;
	WeaponIDEquipped = WeaponInventory[IndexEquippedWeapon].WeaponId;
	OnRefreshWeaponInventory.Broadcast();
}

void UInventoryComponent::SwitchWeapon(int32 Direction) {
	if (Direction == 0) { return; }
	if (WeaponInventory.Num() == 0) { return; }

	if (!bHasEquippedWeapon) {
		IndexEquippedWeapon = 0;
		bHasEquippedWeapon = true;
		WeaponIDEquipped = WeaponInventory[IndexEquippedWeapon].WeaponId;
		OnRefreshWeaponInventory.Broadcast();
		return;
	}
	if (WeaponInventory.Num() > 1) {
		IndexEquippedWeapon += Direction;
		if (IndexEquippedWeapon >= WeaponInventory.Num()) {
			IndexEquippedWeapon = 0;
		}
		if (IndexEquippedWeapon < 0) {
			IndexEquippedWeapon = WeaponInventory.Num() - 1;
		}
		WeaponIDEquipped = WeaponInventory[IndexEquippedWeapon].WeaponId;
		OnRefreshWeaponInventory.Broadcast();
	}
}

// Called when the game starts
void UInventoryComponent::BeginPlay() {
	Super::BeginPlay();
	OnRefreshWeaponInventory.Broadcast();
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
