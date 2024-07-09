// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaylaGameMode.h"
#include "UObject/ConstructorHelpers.h"

ALaylaGameMode::ALaylaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/B_LaylaCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
