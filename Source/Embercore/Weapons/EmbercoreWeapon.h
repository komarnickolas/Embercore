// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbercoreProjectile.h"
#include "EmbercoreWeapon.generated.h"

/**
 * 
 */
USTRUCT()
struct FWeaponData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float Range;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category="Stats")
	int32 Level;

	FWeaponData() {
		Range = 10.0f;
		FireRate = 10.0f;
		Damage = 10.0f;
		Level = 1;
	}
};


UCLASS()
class EMBERCORE_API AEmbercoreWeapon : public AActor {
	GENERATED_BODY()
public:
	float GetFireRate() const;
	float GetDamage() const;
	float GetRange() const;
	TSubclassOf<AEmbercoreProjectile> GetProjectileClass() const;
	TSubclassOf<UGameplayEffect> GetDamageGameplayEffect() const;
	FWeaponData WeaponConfig;
	void OnUnEquip();
	void OnEquip();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AEmbercoreProjectile> ProjectileClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
};
