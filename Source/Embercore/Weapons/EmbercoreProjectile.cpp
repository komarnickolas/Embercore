// Copyright 2020 Dan Kestranek.


#include "EmbercoreProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AEmbercoreProjectile::AEmbercoreProjectile() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AEmbercoreProjectile::BeginPlay() {
	Super::BeginPlay();

	SetLifeSpan(ProjectileMovement->InitialSpeed / Range);
}
