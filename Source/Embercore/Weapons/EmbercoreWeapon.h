// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmbercoreProjectile.h"
#include "NiagaraSystem.h"
#include "EmbercoreWeapon.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct EMBERCORE_API FWeaponStructure : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stats")
	int32 Level;

	FWeaponStructure(): Range(0), FireRate(0), Damage(0), Level(0) {
	}
};


UCLASS()
class EMBERCORE_API AEmbercoreWeapon : public AActor {
	GENERATED_BODY()
public:
	float GetFireRate() const;
	float GetDamage() const;
	float GetRange() const;
	FText GetName() const;
	TSubclassOf<AEmbercoreProjectile> GetProjectileClass() const;
	TSubclassOf<UGameplayEffect> GetDamageGameplayEffect() const;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category=Stats)
	FWeaponStructure WeaponStructure;
	void OnUnEquip();
	void OnEquip();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AEmbercoreProjectile> ProjectileClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
};
