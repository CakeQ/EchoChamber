// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeMaker.generated.h"

UCLASS()
class ECHOCHAMBER_API AMazeMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeMaker();

	UPROPERTY()
		float _tileSize = 100.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UInstancedStaticMeshComponent* WallType;
	UInstancedStaticMeshComponent* FloorType;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnBorders(int Size = 10);

};
