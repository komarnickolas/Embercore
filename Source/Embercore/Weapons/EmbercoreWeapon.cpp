// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercoreWeapon.h"

float AEmbercoreWeapon::GetFireRate() const {
	return WeaponStructure.FireRate;
}

float AEmbercoreWeapon::GetDamage() const {
	return WeaponStructure.Damage;
}

float AEmbercoreWeapon::GetRange() const {
	return WeaponStructure.Range;
}

FText AEmbercoreWeapon::GetName() const {
	return WeaponStructure.Name;
}

TSubclassOf<AEmbercoreProjectile> AEmbercoreWeapon::GetProjectileClass() const {
	return ProjectileClass;
}

TSubclassOf<UGameplayEffect> AEmbercoreWeapon::GetDamageGameplayEffect() const {
	return DamageGameplayEffect;
}

void AEmbercoreWeapon::OnUnEquip() {
}

void AEmbercoreWeapon::OnEquip() {
}
