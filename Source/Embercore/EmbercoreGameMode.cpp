// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmbercoreGameMode.h"
#include "EmbercoreCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEmbercoreGameMode::AEmbercoreGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
