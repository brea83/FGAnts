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

float GridTile::GetPheromoneAmount(EPheromoneTypes pheromone)
{
	float* pheromoneAmount = _pheromones.Find(pheromone);

	if (pheromoneAmount) return *pheromoneAmount;

	// if pheromoneAmount was an invalid pointer return a negative number,
	// functions calling this need to know and check for that a negative number is an invalid result
	return -1.0f;
}

void GridTile::AddPheromoneAmount(EPheromoneTypes pheromone, float amount)
{
	if (amount <= 0) return; // this is not to be used for decrementing pheromone amounts

	float* oldAmount = _pheromones.Find(pheromone);

	if (oldAmount)
	{
		amount += *oldAmount;
	}
	amount = FMath::Min(amount, 60.0f);
	_pheromones.Add(pheromone, amount);
	_bHasPheromones = true;
}

void GridTile::DecayPheromones(float DeltaTime)
{

	TArray<EPheromoneTypes> keys;
	_pheromones.GenerateKeyArray(keys);

	for (EPheromoneTypes key : keys)
	{
		float newValue = FMath::Min(*_pheromones.Find(key) - DeltaTime, 60.0f);

		if (newValue <= 0)
		{
			_pheromones.Remove(key);
		}
		else
		{
			_pheromones.Add(key, newValue);
		}
	}

	if (_pheromones.Num() <= 0)
	{
		_bHasPheromones = false;
	}
	
}
