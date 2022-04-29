// Copyright 2020 Dan Kestranek.


#include "EmbercoreAT_PlayMontageAndWaitForEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Embercore/Abilities/EmbercoreAbilitySystemComponent.h"
#include "GameFramework/Character.h"

UEmbercoreAT_PlayMontageAndWaitForEvent::UEmbercoreAT_PlayMontageAndWaitForEvent(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UEmbercoreAbilitySystemComponent* UEmbercoreAT_PlayMontageAndWaitForEvent::GetTargetASC() {
	return Cast<UEmbercoreAbilitySystemComponent>(AbilitySystemComponent);
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted) {
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay) {
		if (Montage == MontageToPlay) {
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() ==
					EGameplayAbilityNetExecutionPolicy::LocalPredicted))) {
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted) {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::OnAbilityCancelled() {
	// TODO: Merge this fix back to engine, it was calling the wrong callback

	if (StopPlayingMontage()) {
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	if (!bInterrupted) {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag,
                                                              const FGameplayEventData* Payload) {
	if (ShouldBroadcastAbilityTaskDelegates()) {
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UEmbercoreAT_PlayMontageAndWaitForEvent* UEmbercoreAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate,
	FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale) {
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UEmbercoreAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UEmbercoreAT_PlayMontageAndWaitForEvent>(
		OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::Activate() {
	if (Ability == nullptr) {
		return;
	}

	bool bPlayedMontage = false;
	UEmbercoreAbilitySystemComponent* EmbercoreAbilitySystemComponent = GetTargetASC();

	if (EmbercoreAbilitySystemComponent) {
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr) {
			// Bind to event callback
			EventHandle = EmbercoreAbilitySystemComponent->AddGameplayEventTagContainerDelegate(
				EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this, &UEmbercoreAT_PlayMontageAndWaitForEvent::OnGameplayEvent));

			if (EmbercoreAbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(),
			                                                 MontageToPlay, Rate, StartSection) > 0.f) {
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false) {
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(
					this, &UEmbercoreAT_PlayMontageAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UEmbercoreAT_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UEmbercoreAT_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() ==
						EGameplayAbilityNetExecutionPolicy::LocalPredicted))) {
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else {
			UE_LOG(LogTemp, Warning,
			       TEXT("UEmbercoreAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning,
		       TEXT("UEmbercoreAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage) {
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "UEmbercoreAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."
		       ), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates()) {
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::ExternalCancel() {
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UEmbercoreAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded) {
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability) {
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds) {
			StopPlayingMontage();
		}
	}

	UEmbercoreAbilitySystemComponent* EmbercoreAbilitySystemComponent = GetTargetASC();
	if (EmbercoreAbilitySystemComponent) {
		EmbercoreAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UEmbercoreAT_PlayMontageAndWaitForEvent::StopPlayingMontage() {
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo) {
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr) {
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability) {
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay) {
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance) {
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UEmbercoreAT_PlayMontageAndWaitForEvent::GetDebugString() const {
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability) {
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr) {
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay)
				                 ? MontageToPlay
				                 : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(
		TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay),
		*GetNameSafe(PlayingMontage));
}
