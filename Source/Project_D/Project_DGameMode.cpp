// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_DGameMode.h"
#include "Project_DCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_DGameMode::AProject_DGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
