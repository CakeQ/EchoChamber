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

	UPROPERTY(EditAnywhere, Category = "Propagation")
	float radius;
	UPROPERTY(EditAnywhere, Category = "Propagation")
	float speed;
	UPROPERTY(EditAnywhere, Category = "Propagation")
	float acceleration;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	float thickness;
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FColor color;
	UPROPERTY(EditAnywhere, Category = "Appearance")
	int segments;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<FVector> Collisions;

	// DEBUG
	bool bGoing;
};
