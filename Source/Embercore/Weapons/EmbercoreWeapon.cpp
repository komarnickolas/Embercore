// Fill out your copyright notice in the Description page of Project Settings.


#include "EmbercoreWeapon.h"

float AEmbercoreWeapon::GetFireRate() const {
	return WeaponConfig.FireRate;
}

float AEmbercoreWeapon::GetDamage() const {
	return WeaponConfig.Damage;
}

float AEmbercoreWeapon::GetRange() const {
	return WeaponConfig.Range;
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
