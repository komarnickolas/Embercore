// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Embercore/EmbercoreCharacter.h"
#include "GameFramework/Character.h"
#include "BasicEnemy.generated.h"

UCLASS()
class EMBERCORE_API ABasicEnemy : public AEmbercoreCharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicEnemy(const class FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UEmbercoreFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Embercore|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool PlayerDetected;
	bool CanAttackPlayer;

	UPROPERTY(BlueprintReadWrite)
	bool CanDealDamage;

	class APlayerCharacter* PlayerRef;

	UPROPERTY(EditAnywhere)
	class USphereComponent* PlayerCollisionDetection;

	UPROPERTY(EditAnywhere)
	class USphereComponent* PlayerAttackCollisionDetection;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* DamageCollision;

	class ABasicEnemyAIController* BasicEnemyAIController;

	void OnAIMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);

	UPROPERTY(EditAnywhere)
	float StoppingDistance = 100.0f;

	FTimerHandle SeekPlayerTimeHandle;

	UFUNCTION()
	void SeekPlayer();

	UFUNCTION()
	void StopSeekingPlayer();

	UFUNCTION()
	void OnPlayerDetectedOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	                                  class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp,
	                                class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex);

	UFUNCTION()
	void OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	                                class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
	                              class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                              int32 OtherBodyIndex);

	UFUNCTION()
	void OnDealDamageOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	                              class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
