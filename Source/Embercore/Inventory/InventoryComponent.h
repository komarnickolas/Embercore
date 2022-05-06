// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStructure.h"
#include "Components/ActorComponent.h"
#include "Embercore/Weapons/WeaponData.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EMBERCORE_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_OneParam(UInventoryComponent, FItemAddedEvent, FItemStructure)

	FItemAddedEvent& OnAddItem() { return ItemAddedEvent; }

	UFUNCTION()
	bool AddItem(FString Name, FString Description, TSubclassOf<AActor> Actor);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	FString InventoryName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	int32 Capacity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	TArray<FItemStructure> Inventory;
	FItemAddedEvent ItemAddedEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Weapons")
	TArray<FWeaponDataStructure> WeaponInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Weapons")
	int32 WeaponCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Weapons")
	bool bHasEquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Weapons")
	int32 IndexEquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Weapons")
	FName WeaponIDEquipped;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Events")
	void OnRefreshWeaponInventory();

	void AddWeapon(FName WeaponID);
	int32 FindWeaponInventoryIndex(FName WeaponID);
	void RemoveWeapon(FName WeaponID, bool RemoveEquippedWeapon);
	bool HasFreeWeaponSlot();
	bool HasEquippedWeapon(FName WeaponID);
	TSubclassOf<AEmbercoreWeapon> GetCurrentlyEquippedWeapon();
	void SwitchWeapon(int32 Direction);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


};
