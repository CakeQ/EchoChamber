// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "EchoChamberGameMode.h"
#include "EchoChamberPlayerController.h"
#include "EchoChamberCharacter.h"
#include "Engine/World.h"
#include "Engine.h"
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

	LevelTimeLimit = 10.0f;
}

void AEchoChamberGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartLevelTimer(LevelTimeLimit);
}

void AEchoChamberGameMode::StartLevelTimer(float TimeLimit)
{
	GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &AEchoChamberGameMode::OnLevelTimerExpire, TimeLimit, false, TimeLimit);
}

float AEchoChamberGameMode::GetRemainingTime()
{
	return GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
}

void AEchoChamberGameMode::AddLevelTime(float Time)
{
	float Remaining = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle) + Time;
	GetWorldTimerManager().SetTimer(LevelTimerHandle, this, &AEchoChamberGameMode::OnLevelTimerExpire, -1.0f, false, Remaining + Time);
}

void AEchoChamberGameMode::OnLevelTimerExpire()
{
	// Clear the timer to prevent it from re-firing
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	UE_LOG(LogClass, Warning, TEXT("LICK ASS"));
}