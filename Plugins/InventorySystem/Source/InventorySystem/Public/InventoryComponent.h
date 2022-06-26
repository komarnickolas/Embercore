// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStructure.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT_TwoParams(UInventoryComponent, FItemAddedEvent, FItemStructure, int32)

	FItemAddedEvent ItemAddedEvent;

	FItemAddedEvent& OnAddItem() { return ItemAddedEvent; }

	UFUNCTION(BlueprintCallable)
	int32 AddItem(FItemStructure InItem);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	FString InventoryName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	int32 Capacity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory|Items")
	TArray<FItemStructure> Inventory;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


};
