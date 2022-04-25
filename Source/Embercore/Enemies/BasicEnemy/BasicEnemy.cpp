// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemy.h"

#include "BasicEnemyAIController.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Embercore/EmbercoreCharacter.h"
#include "Embercore/UI/EmbercoreFloatingStatusBarWidget.h"

// Sets default values
ABasicEnemy::ABasicEnemy(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	PlayerCollisionDetection->SetupAttachment(RootComponent);

	PlayerAttackCollisionDetection = CreateDefaultSubobject<
		USphereComponent>(TEXT("Player Attack Collision Detection"));
	PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(GetMesh(), TEXT("RootSocket"));

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(50, 2));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr,
	                                           TEXT(
		                                           "/Game/UI/UI_FloatingStatusBar_Player.UI_FloatingStatusBar_Player_C"));
	if (!UIFloatingStatusBarClass) {
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."
		       ), *FString(__FUNCTION__));
	}
	UIFloatingStatusBarComponent->SetWidgetClass(UIFloatingStatusBarClass);
}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay() {
	Super::BeginPlay();

	BasicEnemyAIController = Cast<ABasicEnemyAIController>(GetController());
	BasicEnemyAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
		this, &ABasicEnemy::OnAIMoveCompleted);

	PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerDetectedOverlapBegin);
	PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerDetectedOverlapEnd);

	PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerAttackOverlapBegin);
	PlayerAttackCollisionDetection->OnComponentEndOverlap.AddDynamic(this, &ABasicEnemy::OnPlayerAttackOverlapEnd);

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicEnemy::OnDealDamageOverlapBegin);
}

// Called every frame
void ABasicEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABasicEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABasicEnemy::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	if (!PlayerDetected) {
		BasicEnemyAIController->RandomPatrol();
	}
	else if (PlayerDetected && CanAttackPlayer) {
		StopSeekingPlayer();
		UE_LOG(LogTemp, Warning, TEXT("Player ATTACKED"));
	}
}

void ABasicEnemy::MoveToPlayer() {
	BasicEnemyAIController->MoveToLocation(PlayerRef->GetActorLocation(), StoppingDistance, true);
}

void ABasicEnemy::SeekPlayer() {
	MoveToPlayer();
	GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimeHandle, this, &ABasicEnemy::SeekPlayer, 0.25f, true);
}

void ABasicEnemy::StopSeekingPlayer() {
	GetWorld()->GetTimerManager().ClearTimer(SeekPlayerTimeHandle);
}

void ABasicEnemy::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult) {
	PlayerRef = Cast<AEmbercoreCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = true;
		SeekPlayer();
	}
}

void ABasicEnemy::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	PlayerRef = Cast<AEmbercoreCharacter>(OtherActor);
	if (PlayerRef) {
		PlayerDetected = false;
		StopSeekingPlayer();
		BasicEnemyAIController->RandomPatrol();
	}

}

void ABasicEnemy::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult) {
	PlayerRef = Cast<AEmbercoreCharacter>(OtherActor);
	if (PlayerRef) {
		CanAttackPlayer = true;
	}
}

void ABasicEnemy::OnPlayerAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	PlayerRef = Cast<AEmbercoreCharacter>(OtherActor);
	if (PlayerRef) {
		CanAttackPlayer = false;
		SeekPlayer();
	}
}

void ABasicEnemy::OnDealDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult) {
	PlayerRef = Cast<AEmbercoreCharacter>(OtherActor);
	if (PlayerRef && CanDealDamage) {
		UE_LOG(LogTemp, Warning, TEXT("Player Damaged"));
	}
}
