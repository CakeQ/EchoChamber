// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeMaker.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class ECHOCHAMBER_API AMazeMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeMaker();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Maze, meta = (AllowPrivateAccess = "true"))
	float _tileSize = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Maze, meta = (AllowPrivateAccess = "true"))
	int32 _mazeSize = 20;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UInstancedStaticMeshComponent* WallType;
	UInstancedStaticMeshComponent* FloorType;

	FVector CurrentTile;

	TArray<int> WallTiles;
	TArray<int> FloorTiles;
	TArray<int> BridgeTiles;
	TArray<FVector> EndTiles;

	int VectorToGrid(FVector v);

	bool HasTile(int TilePos);
	bool CheckBridge(FVector& CurTile, int dir = 0);

	void SpawnMaze(int Size = 10);
	void MakeBridge(FVector& CurTile, int dir = 0);

	void AddWall(FTransform& TileTransform);
	void AddFloor(FTransform& TileTransform);
	void AddBridge(FTransform& TileTransform);
	void AddStart(FTransform& CurTile);
	void AddEnd(FTransform& CurTile);
	void PickEnd();
};
