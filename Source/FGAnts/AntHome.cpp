// Fill out your copyright notice in the Description page of Project Settings.


#include "AntHome.h"
#include "AntGrid.h"
#include "GridTile.h"

// Sets default values
AAntHome::AAntHome()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAntHome::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAntHome::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (_bWaitingForGrid)
	{
		if (!TryInitialize()) return;
	}

	float homeStrength = _currentTile->GetPheromoneAmount(EPheromoneTypes::Home);
	if (homeStrength < 0)
	{
		_currentTile->AddPheromoneAmount(EPheromoneTypes::Home, DeltaTime );

		TArray<GridTile*> neighbors = _grid->GetNeighborTiles(_currentTile);

		for (GridTile* tile : neighbors)
		{
			tile->AddPheromoneAmount(EPheromoneTypes::Home, DeltaTime );
		}
	}
}

