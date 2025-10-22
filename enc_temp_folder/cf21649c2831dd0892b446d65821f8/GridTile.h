// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	TMap<FName, float> _pheromones;

public: // functions
	//virtual void Tick(float DeltaTime);

	const bool IsTraversable() const { return _bIsTraversable; }
	void SetIsTraversable(bool value) { _bIsTraversable = value; }

	const bool HasPheromones() const { return _bHasPheromones; }

	float GetPheromoneAmount(FName pheromoneName);

	void SetPheromoneAmount(FName pheromoneName, float amount);

	const FIntVector2 GetCoord() const { return _coord; }

};
