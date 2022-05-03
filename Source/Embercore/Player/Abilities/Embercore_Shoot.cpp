// Fill out your copyright notice in the Description page of Project Settings.

#include "Embercore_Shoot.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Camera/CameraComponent.h"
#include "Embercore/Abilities/Tasks/EmbercoreAT_PlayMontageAndWaitForEvent.h"
#include "Embercore/Abilities/Tasks/EmbercoreAT_WaitReceiveDamage.h"
#include "Embercore/Player/PlayerCharacter.h"
#include "Embercore/Weapons/EmbercoreProjectile.h"
#include "WebBrowser/Private/CEF/CEFTextInputMethodContext.h"

UEmbercore_Shoot::UEmbercore_Shoot() {
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag ShootTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Shoot"));
	AbilityTags.AddTag(ShootTag);
	ActivationOwnedTags.AddTag(ShootTag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	Range = 1000.0f;
	Damage = 12.0f;
}

void UEmbercore_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData) {

	UE_LOG(LogTemp, Warning, TEXT("Shoot"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = FireMontage;

	// Play fire montage and wait for event telling us to spawn the projectile
	UEmbercoreAT_PlayMontageAndWaitForEvent* Task = UEmbercoreAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
		this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UEmbercore_Shoot::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UEmbercore_Shoot::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UEmbercore_Shoot::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UEmbercore_Shoot::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UEmbercore_Shoot::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UEmbercore_Shoot::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData) {
	UE_LOG(LogTemp, Warning, TEXT("Cancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UEmbercore_Shoot::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData) {
	UE_LOG(LogTemp, Warning, TEXT("Completed"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UEmbercore_Shoot::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData) {
	UE_LOG(LogTemp, Warning, TEXT("Event Recieved"));
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility"))) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(
		FName("Event.Montage.SpawnProjectile"))) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
		if (!Player) {
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
		FRotator Rotation = Player->GetActorRotation();

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(
			DamageGameplayEffect, GetAbilityLevel());

		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);

		FTransform MuzzleTransform = Player->GetMesh()->GetSocketTransform(FName("MuzzleStationary"));
		MuzzleTransform.SetRotation(Rotation.Quaternion());
		MuzzleTransform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AEmbercoreProjectile* Projectile = GetWorld()->SpawnActorDeferred<AEmbercoreProjectile>(
			ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
			Player, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
		Projectile->Range = Range;
		Projectile->FinishSpawning(MuzzleTransform);
	}
}
