// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Embercore/EmbercoreCharacter.h"
#include "EmbercoreProjectile.generated.h"

UCLASS()
class EMBERCORE_API AEmbercoreProjectile : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEmbercoreProjectile();

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<AEmbercoreCharacter> Owner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	float Range;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
