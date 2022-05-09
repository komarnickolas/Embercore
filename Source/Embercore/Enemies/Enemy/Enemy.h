// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Embercore/Abilities/AsyncTask/AsyncTaskAttributeChanged.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class EMBERCORE_API AEnemy : public AEmbercoreCharacter {
	GENERATED_BODY()

public:
	AEnemy(const class FObjectInitializer& ObjectInitializer);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY(BlueprintReadOnly)
	class UEmbercoreAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY(BlueprintReadOnly)
	class UEmbercoreAttributeSet* HardRefAttributeSetBase;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Embercore|UI")
	TSubclassOf<class UEmbercoreFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UEmbercoreFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Embercore|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};
