// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Echo.generated.h"

UCLASS()
class ECHOCHAMBER_API AEcho : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEcho();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Propagation)
	float Radius;
	UPROPERTY(EditAnywhere, Category = Propagation)
	float Speed;
	UPROPERTY(EditAnywhere, Category = Propagation)
	float Acceleration;
	UPROPERTY(EditAnywhere, Category = Propagation)
	float StrengthLossRate;
	UPROPERTY(EditAnywhere, Category = Propagation)
	float StrengthBounceMultiplier;
	UPROPERTY(VisibleAnywhere, Category = Propagation)
	float Strength;

	UPROPERTY(EditAnywhere, Category = Appearance)
	float Thickness;
	UPROPERTY(EditAnywhere, Category = Appearance)
	FColor Color;
	UPROPERTY(EditAnywhere, Category = Appearance)
	int Segments;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = Tracing)
	float SweepAngle;
	UPROPERTY(EditAnywhere, Category = Tracing)
	float MaxTraceDist;
	UPROPERTY(EditAnywhere, Category = Tracing)
	int TracesPerSweep;

	void SetSpawnSurfaceActor(AActor* Actor);
	void SetEchoStrength(float NewStrength);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// The actor whose surface this echo was spawned on, if any
	AActor* SpawnedOn;

	// Called when the echo should bounce off another actor
	void OnEchoBounce(FVector HitLocation, AActor* HitActor);
};
