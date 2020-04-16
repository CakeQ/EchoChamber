// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECEchoEmitter.generated.h"

class AStaticMeshActor;
class USphereComponent;
class UParticleSystemComponent;

static const int NumTraces = 60;
static const int MaxNumBounces = 6;
static const float DeltaAngle = 360.0f / NumTraces;
static const float TraceSpeed = 500.0f;
static const float MaxTraceDistance = 1000.0f;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Echo)
		TSubclassOf<AActor> TravellingParticleActor;

	UFUNCTION(BlueprintNativeEvent, Category = "EchoBounce")
		void OnWallActorHit(AActor* ActorHit);

private:
	struct EchoSegment
	{
		int BouncesRemaining;
		float DistanceRemaining;
		float Speed;
		FVector Origin;
		FVector Direction;
		bool Done;
		AActor* ParticleActor;
	};

	EchoSegment Segments[NumTraces];
};



