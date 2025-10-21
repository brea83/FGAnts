// Fill out your copyright notice in the Description page of Project Settings.


#include "AntGrid.h"
#include "AntTile.h"

AAntGrid::AAntGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AAntGrid::InitializeGrid()
{
	FVector origin = GetActorLocation() + (FVector(TileSize.X, TileSize.Y, 0) * 0.5f);
	FRotator spawnRotation = GetActorRotation();

	for (int x = 0; x < GridSize.X ; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			FVector spawnLocation = FVector(origin.X + (x * TileSize.X), origin.Y + (y * TileSize.Y), origin.Z);
			FActorSpawnParameters spawnParams;
			
			AAntTile* newTile = GetWorld()->SpawnActor<AAntTile>(spawnLocation, spawnRotation, spawnParams);
			newTile->halfWidth = TileSize.X * 0.5;
			newTile->halfHeight = TileSize.Y * 0.5;

			Tiles.Add(newTile);
		}

	}
}

void AAntGrid::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
}

void AAntGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AAntTile* AAntGrid::GetClosestTile(FVector queryPosition)
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

