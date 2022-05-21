// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EMBERCORE_API UInteractableComponent : public UWidgetComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractableComponent();
	UPROPERTY(Category=EventDispatchers, BlueprintAssignable)
	FOnInteract OnInteractEvent;
	UFUNCTION(BlueprintCallable)
	void ToggleInput(APlayerController* Controller, bool InToggle);
	UFUNCTION(BlueprintCallable)
	void BindToInput(FName ActionName, bool Enable);
	UFUNCTION(BlueprintCallable)
	void StartTimer();
	UFUNCTION(BlueprintCallable)
	void StopTimer();
	UFUNCTION(BlueprintCallable)
	void UpdateTimer();
	UFUNCTION(BlueprintCallable)
	void OnInteractPressed();
	UFUNCTION(BlueprintCallable)
	void DelayInteraction();
	void TriggerInteract();
	UFUNCTION(BlueprintCallable)
	void OnInteractReleased();
	FTimerHandle InteractTimer;
	FTimerHandle UpdateTimerHandle;
	FTimerHandle DelayInteractionTimerHandle;
	UPROPERTY(BlueprintReadWrite)
	bool IsHeld = false;
	UPROPERTY(BlueprintReadWrite)
	bool Active;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxHoldTime = 1.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
