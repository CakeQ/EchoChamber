// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeMaker.h"
#include "Engine/StaticMesh.h"

// Sets default values
AMazeMaker::AMazeMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMazeMaker::BeginPlay()
{
	UStaticMesh* Wall = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/StarterContent/Shapes/Shape_Cube")));
	WallType = NewObject<UInstancedStaticMeshComponent>(this);
	WallType->RegisterComponent();
	WallType->SetStaticMesh(Wall);
	WallType->SetFlags(RF_Transactional);
	this->AddInstanceComponent(WallType);

	UStaticMesh* Floor = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/StarterContent/Shapes/Shape_Plane")));
	FloorType = NewObject<UInstancedStaticMeshComponent>(this);
	FloorType->RegisterComponent();
	FloorType->SetStaticMesh(Wall);
	FloorType->SetFlags(RF_Transactional);
	this->AddInstanceComponent(FloorType);

	this->SpawnBorders();
}

void AMazeMaker::OnConstruction(const FTransform& Transform)
{
}

// Called every frame
void AMazeMaker::Tick(float DeltaTime)
{

}

void AMazeMaker::SpawnBorders(int Size /* = 10 */)
{
	for (int width = 0; width < Size; width++)
	{
		for (int height = 0; height < Size; height++)
		{
			if (width == 1 || width == Size - 1 || height == 0 || height == Size - 1)
			{
				FTransform t(FVector(width * _tileSize, height * _tileSize, 0));
				WallType->AddInstance(t);
			}
		}
	}
}
