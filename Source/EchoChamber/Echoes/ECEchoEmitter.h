// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECEchoEmitter.generated.h"

class USphereComponent;

static const int NumTraces = 60;
static const int MaxNumBounces = 6;
static const float DeltaAngle = 360.0f / NumTraces;
static const float TraceSpeed = 300.0f;
static const float MaxTraceDistance = 600.0f;

UCLASS()
class AECEchoEmitter : public AActor
{
	GENERATED_BODY()

public:
	AECEchoEmitter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	USphereComponent* SphereComponent;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Echo)
		UParticleSystem* ParticleSystem;

private:
	struct EchoSegment
	{
		int BouncesRemaining;
		float DistanceRemaining;
		float Speed;
		FVector Origin;
		FVector Direction;
		bool Done;
	};

	EchoSegment Segments[NumTraces];
};



