// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTile.h"

GridTile::GridTile()
{
}

GridTile::GridTile(FVector position, FIntVector2 coord, bool traversable)
	: _centerPosition(position) , _coord(coord), _bIsTraversable (traversable), _bHasPheromones(false)
{}

GridTile::~GridTile()
{
	Occupants.Empty();
	_pheromones.Empty();
}

float GridTile::GetPheromoneAmount(FName pheromoneName)
{
	float* pheromoneAmount = _pheromones.Find(pheromoneName);

	if (pheromoneAmount) return *pheromoneAmount;

	// if pheromoneAmount was an invalid pointer return a negative number,
	// functions calling this need to know and check for that a negative number is an invalid result
	return -1.0f;
}

void GridTile::SetPheromoneAmount(FName pheromoneName, float amount)
{
	if (amount <= 0) return; // this is not to be used for decrementing pheromone amounts

	float* oldAmount = _pheromones.Find(pheromoneName);

	if (oldAmount)
	{
		amount += *oldAmount;
	}

	_pheromones.Add(pheromoneName, amount);
	_bHasPheromones = true;
}
