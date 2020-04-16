// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeMaker.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerStart.h"
#include "Public/EchoFinishLine.h"

// Sets default values
AMazeMaker::AMazeMaker()
{

}

// Called when the game starts or when spawned
void AMazeMaker::BeginPlay()
{
	UStaticMesh* Wall = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/StarterContent/Shapes/Shape_Cube")));
	WallType = NewObject<UInstancedStaticMeshComponent>(this);
	WallType->RegisterComponent();
	WallType->SetStaticMesh(Wall);
	WallType->SetFlags(RF_Transactional);
	this->AddInstanceComponent(WallType);

	UStaticMesh* Floor = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/StarterContent/Shapes/Shape_Plane")));
	FloorType = NewObject<UInstancedStaticMeshComponent>(this);
	FloorType->RegisterComponent();
	FloorType->SetStaticMesh(Floor);
	FloorType->SetFlags(RF_Transactional);
	this->AddInstanceComponent(FloorType);

	this->SpawnMaze(_mazeSize);
}

int AMazeMaker::VectorToGrid(FVector t)
{
	return (t.Y * _mazeSize) + t.X;
}

bool AMazeMaker::HasTile(int TilePos)
{
	if (WallTiles.Contains(TilePos) || FloorTiles.Contains(TilePos) || BridgeTiles.Contains(TilePos))
	{
		return true;
	}
	return false;
}

bool AMazeMaker::CheckBridge(FVector& CurTile, int dir /* = 0 */)
{
	FVector BridgeCheck(CurTile.X, CurTile.Y, CurTile.Z);
	FVector TileCheck(CurTile.X, CurTile.Y, CurTile.Z);
	switch (dir) {
	case 0:
		BridgeCheck.Y += 1 * _tileSize;
		TileCheck.Y += 2 * _tileSize;
		break;
	case 1:
		BridgeCheck.X += 1 * _tileSize;
		TileCheck.X += 2 * _tileSize;
		break;
	case 2:
		BridgeCheck.Y += -1 * _tileSize;
		TileCheck.Y += -2 * _tileSize;
		break;
	case 3:
		BridgeCheck.X += -1 * _tileSize;
		TileCheck.X += -2 * _tileSize;
		break;
	default:
		BridgeCheck.Y += 1 * _tileSize;
		TileCheck.Y += 2 * _tileSize;
		break;
	}

	FTransform BridgeTransform(BridgeCheck);
	FTransform TileTransform(TileCheck);

	if (HasTile(VectorToGrid(BridgeCheck)) || HasTile(VectorToGrid(TileCheck))) {
		return true;
	}
	return false;
}

void AMazeMaker::MakeBridge(FVector& CurTile, int dir /* = 0 */)
{
	FVector BridgeLoc(CurTile.X, CurTile.Y, CurTile.Z);
	FVector TileLoc(CurTile.X, CurTile.Y, CurTile.Z);
	switch (dir) {
	case 0:
		BridgeLoc.Y += 1 * _tileSize;
		TileLoc.Y += 2 * _tileSize;
		break;
	case 1:
		BridgeLoc.X += 1 * _tileSize;
		TileLoc.X += 2 * _tileSize;
		break;
	case 2:
		BridgeLoc.Y += -1 * _tileSize;
		TileLoc.Y += -2 * _tileSize;
		break;
	case 3:
		BridgeLoc.X += -1 * _tileSize;
		TileLoc.X += -2 * _tileSize;
		break;
	default:
		BridgeLoc.Y += 1 * _tileSize;
		TileLoc.Y += 2 * _tileSize;
		break;
	}

	FTransform BridgeTransform(BridgeLoc);
	FTransform TileTransform(TileLoc);

	AddBridge(BridgeTransform);
	AddFloor(TileTransform);

	CurrentTile = TileLoc;
}

void AMazeMaker::SpawnMaze(int Size /* = 10 */)
{
	//Spawn the borders with offsets to not take up the internal size define
	for (int width = -1; width <= Size; width++)
	{
		for (int height = -1; height <= Size; height++)
		{
			FVector v(width * _tileSize, height * _tileSize, 0);
			FTransform t(v);
			if (width == -1 || width == Size || height == -1 || height == Size)
			{
				WallTiles.AddUnique(VectorToGrid(v));
				WallType->AddInstance(t);
			}
		}
	}

	//Setup Maze building vars
	bool HasSpace = true;
	CurrentTile = FVector(0, 0, 0);
	TArray<FVector> CurrentPath;
	FTransform StartTransform(CurrentTile);
	AddStart(StartTransform);
	CurrentPath.Add(CurrentTile);

	while (HasSpace)
	{
		//Create an array of possible directions to branch
		TArray<int> DirList;
		for (int dir = 0; dir <= 3; dir++)
		{
			if (!CheckBridge(CurrentTile, dir))
			{
				DirList.Add(dir);
			}
		}
		//If there are no possible directions, backtrack
		if (!DirList.Num())
		{
			FTransform EndTransform(CurrentTile);
			AddEnd(EndTransform);
			//If backtracking is no longer possible, abort
			if (!CurrentPath.Num()) {
				HasSpace = false;
				break;
			}
			CurrentTile = CurrentPath.Pop();
			continue;
		}
		//Pick a random direction to branch to
		int DirToUse = DirList[FMath::RandRange(0, DirList.Num() - 1)];
		CurrentPath.Add(CurrentTile);
		MakeBridge(CurrentTile, DirToUse);
	}

	//Fill in the rest
	for (int width = -1; width <= Size; width++)
	{
		for (int height = -1; height <= Size; height++)
		{
			FVector v(width * _tileSize, height * _tileSize, 0);
			if (HasTile(VectorToGrid(v)))
			{
				continue;
			}
			FTransform t(v);
			AddWall(t);
		}
	}

	PickEnd();
}

void AMazeMaker::AddWall(FTransform& TileTransform)
{
	WallTiles.AddUnique(VectorToGrid(TileTransform.GetLocation()));
	WallType->AddInstance(TileTransform);
}

void AMazeMaker::AddFloor(FTransform& TileTransform)
{
	FloorTiles.AddUnique(VectorToGrid(TileTransform.GetLocation()));
	FloorType->AddInstance(TileTransform);
}

void AMazeMaker::AddBridge(FTransform& TileTransform)
{
	BridgeTiles.AddUnique(VectorToGrid(TileTransform.GetLocation()));
	FloorType->AddInstance(TileTransform);
}

void AMazeMaker::AddStart(FTransform& TileTransform)
{
	//GetWorld()->SpawnActor<APlayerStart>(GetClass(), TileTransform);
	FloorTiles.AddUnique(VectorToGrid(TileTransform.GetLocation()));
	FloorType->AddInstance(TileTransform);
}

void AMazeMaker::AddEnd(FTransform& TileTransform)
{
	EndTiles.AddUnique(TileTransform.GetLocation());
	FloorTiles.AddUnique(VectorToGrid(TileTransform.GetLocation()));
	FloorType->AddInstance(TileTransform);
}

void AMazeMaker::PickEnd()
{
	FVector& TileLoc = EndTiles[FMath::RandRange(0, EndTiles.Num() - 1)];
	//GetWorld()->SpawnActor<AEchoFinishLine>(GetClass(), FTransform(FVector(TileLoc.X, TileLoc.Y, 20.f)));
}
