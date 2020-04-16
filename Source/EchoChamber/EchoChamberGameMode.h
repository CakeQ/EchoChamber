// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EchoChamberGameMode.generated.h"

UCLASS(minimalapi)
class AEchoChamberGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEchoChamberGameMode();

	UFUNCTION()
	void StartLevelTimer(float TimeLimit);

	UFUNCTION()
	void AddLevelTime(float Time);

	UFUNCTION(BlueprintCallable)
	float GetRemainingTime();

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnLevelTimerExpire();

	UPROPERTY(VisibleAnywhere)
	FTimerHandle LevelTimerHandle;

	UPROPERTY(EditAnywhere, Category = Levels)
	float LevelTimeLimit;
};
