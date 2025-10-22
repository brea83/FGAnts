// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PheromoneTypes.h"
#include "CoreMinimal.h"

/**
 * 
 */
class FGANTS_API GridTile
{
public: // constructors and properties
	GridTile();
	GridTile(FVector position, FIntVector2 coord, bool traversable);
	~GridTile();

	TArray<AActor*> Occupants;

protected:

	bool _bHasPheromones{ false };
	bool _bIsTraversable{ true };

	FVector _centerPosition{ 0, 0, 0 };
	FIntVector2 _coord{ 0, 0 };

	TMap<EPheromoneTypes, float> _pheromones;

public: // functions
	//virtual void Tick(float DeltaTime);

	const bool IsTraversable() const { return _bIsTraversable; }
	void SetIsTraversable(bool value) { _bIsTraversable = value; }

	const bool HasPheromones() const { return _bHasPheromones; }

	float GetPheromoneAmount(EPheromoneTypes pheromone);

	void AddPheromoneAmount(EPheromoneTypes pheromone, float amount);

	void DecayPheromones(float DeltaTime);

	const FIntVector2 GetCoord() const { return _coord; }
	const FVector GetCenterPosition() const { return _centerPosition; }

	friend bool operator<(const GridTile& left, const GridTile& right)
	{
		return left._coord.X < right._coord.X && left._coord.Y < right._coord.Y;
	}

	friend bool operator> (const GridTile& lhs, const GridTile& rhs) { return rhs < lhs; }
	friend bool operator<=(const GridTile& lhs, const GridTile& rhs) { return !(lhs > rhs); }
	friend bool operator>=(const GridTile& lhs, const GridTile& rhs) { return !(lhs < rhs); }

	friend bool operator==(const GridTile& lhs, const GridTile& rhs) { return lhs._coord == rhs._coord; }
	friend bool operator!=(const GridTile& lhs, const GridTile& rhs) { return !(lhs == rhs); }
};
