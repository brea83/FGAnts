// Fill out your copyright notice in the Description page of Project Settings.


#include "AntGrid.h"
#include "GridTile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "AntGameState.h"

AAntGrid::AAntGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AAntGrid::InitializeGrid()
{
	FVector origin = GetActorLocation() + (FVector(TileSize.X, TileSize.Y, 0) * 0.5f);
	//FRotator spawnRotation = GetActorRotation();

	for (int x = 0; x < GridSize.X ; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			FVector spawnLocation = FVector(origin.X + (x * TileSize.X), origin.Y + (y * TileSize.Y), origin.Z);
			//FActorSpawnParameters spawnParams;
			
			//AAntTile* newTile = GetWorld()->SpawnActor<AAntTile>(spawnLocation, spawnRotation, spawnParams);
			//newTile->halfWidth = TileSize.X * 0.5;
			//newTile->halfHeight = TileSize.Y * 0.5;
			bool bIsTraversable = IsTileTraversable(spawnLocation);
			if (bIsTraversable)
			{
				DrawDebugBox(GetWorld(), spawnLocation, FVector(TileSize.X * 0.4, TileSize.Y * 0.4, 2), FColor::Emerald, true, -1, 0, 10);
			}
			else
			{
				DrawDebugBox(GetWorld(), spawnLocation, FVector(TileSize.X * 0.4, TileSize.Y * 0.4, 2), FColor::Red, true, -1, 0, 10);
			}

			Tiles.Add(new GridTile(spawnLocation, FIntVector2(x, y), bIsTraversable));

		}

	}
}

void AAntGrid::BeginPlay()
{
	Super::BeginPlay();

	AAntGameState* gameState = Cast<AAntGameState, AGameStateBase>(GetWorld()->GetGameState());
	if (!gameState) { return; }

	// singleton behavior for now
	AAntGrid* activeGrid = gameState->GetActiveGrid();
	if (activeGrid != nullptr && activeGrid != this)
	{
		Destroy();
		return;
	}

	InitializeGrid();
	
	gameState->SetActiveGrid(this);

}

bool AAntGrid::IsTileTraversable(FVector centerPosition)
{
	bool bHitFound{ false };
	FHitResult outHit;
	FVector start = centerPosition;
	FVector end = start + (GetActorUpVector() * 100);
	FVector halfSize{ TileSize.X * 0.4, TileSize.Y * 0., 5 };
	FRotator orientation = GetActorRotation();
	//ECollisionChannel blockersChannel = ECollisionChannel::ECC_WorldStatic;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	bHitFound = UKismetSystemLibrary::BoxTraceSingleByProfile(
		GetWorld(),
		start,
		end,
		halfSize,
		orientation,
		FName("BlockAll"),
		false,
		actorsToIgnore,
		EDrawDebugTrace::None,
		outHit,
		true
	);

	return !bHitFound;
}

GridTile* AAntGrid::GetTileByCoord(FIntVector2 coord)
{
	// y * gridWidth + x = array index of point (x,y)
	return Tiles[coord.Y * GridSize.X + coord.X];
}

void AAntGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (GridTile* tile : Tiles)
	{
		if (tile->HasPheromones())
		{
			tile->DecayPheromones(DeltaTime);
		}
	}
}

GridTile* AAntGrid::GetClosestTile(FVector queryPosition)
{
	FVector2D closestTilePos = FVector2D(
		(TileSize.X / 2) + TileSize.X * floorf(queryPosition.X / TileSize.X),
		(TileSize.Y / 2) + TileSize.Y * floorf(queryPosition.Y / TileSize.Y));
//		queryPosition.Z);
	
	int32 x = GridSize.X + 0.5f - (closestTilePos.X / TileSize.X);
	int32 y = GridSize.Y + 0.5f - (closestTilePos.Y / TileSize.Y);

	// y * gridWidth + x = array index of point (x,y)


	return Tiles[y * GridSize.X + x];
}

TArray<GridTile*> AAntGrid::GetNeighborTiles(GridTile* currentTile)
{
	FIntVector2 coord = currentTile->GetCoord();
	TArray<GridTile*> neighbors;
	bool hasWest = coord.X >= 1;
	bool hasEast = coord.X < GridSize.X - 1;
	bool hasNorth = coord.Y < GridSize.Y - 1;
	bool hasSouth = coord.Y >= 1;

	if (hasNorth)
	{
		neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.N)));
	}

	if (hasSouth)
	{
		neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.S)));
	}

	if (hasWest)
	{
		neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.W)));

		if (hasNorth)
		{
			neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.NW)));
		}

		if (hasSouth)
		{
			neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.SW)));
		}
	}
	if (hasEast)
	{
		neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.E)));

		if (hasNorth)
		{
			neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.NE)));
		}

		if (hasSouth)
		{
			neighbors.Add(GetTileByCoord(FIntVector2(coord + _neighborCoords.SE)));
		}
	}

	return neighbors;
}

