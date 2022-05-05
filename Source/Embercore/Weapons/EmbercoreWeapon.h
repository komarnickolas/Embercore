// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbercoreProjectile.h"
#include "EmbercoreWeapon.generated.h"

/**
 * 
 */

UCLASS()
class EMBERCORE_API AEmbercoreWeapon : public AActor {
	GENERATED_BODY()
public:
	float GetFireRate() const;
	float GetDamage() const;
	float GetRange() const;
	TSubclassOf<AEmbercoreProjectile> GetProjectileClass() const;
	TSubclassOf<UGameplayEffect> GetDamageGameplayEffect() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Level;
	void OnUnEquip();
	void OnEquip();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AEmbercoreProjectile> ProjectileClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
};
