// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStructure.h"
#include "Components/ActorComponent.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory")
	FString InventoryName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory")
	int32 Capacity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory")
	TArray<FItemStructure> Inventory;
	FItemAddedEvent ItemAddedEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


};
