// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "EchoChamberGameMode.h"
#include "EchoChamberPlayerController.h"
#include "EchoChamberCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEchoChamberGameMode::AEchoChamberGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AEchoChamberPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}