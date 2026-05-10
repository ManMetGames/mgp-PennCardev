// Copyright Epic Games, Inc. All Rights Reserved.

#include "themechaniccGameMode.h"
#include "themechaniccCharacter.h"
#include "UObject/ConstructorHelpers.h"

AthemechaniccGameMode::AthemechaniccGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
