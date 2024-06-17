// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaylaGameMode.h"
#include "LaylaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALaylaGameMode::ALaylaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
