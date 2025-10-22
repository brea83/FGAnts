// Fill out your copyright notice in the Description page of Project Settings.


#include "Ant.h"
#include "AntGrid.h"
#include "GridTile.h"
#include "AntGameState.h"

// Sets default values
AAnt::AAnt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnt::BeginPlay()
{
	Super::BeginPlay();

	TryInitialize();
}

bool AAnt::TryInitialize()
{
	AAntGameState* gameState = Cast<AAntGameState, AGameStateBase>(GetWorld()->GetGameState());
	if (!gameState) 
	{
		_bWaitingForGrid = true;
		return false; 
	}

	_grid = gameState->GetActiveGrid();
	if (_grid == nullptr)
	{
		_bWaitingForGrid = true;
		return false;
	}

	_currentTile = _grid->GetClosestTile(GetActorLocation());

	if (_currentTile != nullptr)
	{
		_currentTile->Occupants.Add(this);
		CurrentDestination = _currentTile->GetCenterPosition();
		_bWaitingForGrid = false;
		return true;
	}

	return false;
	
}

// Called every frame
void AAnt::Tick(float DeltaTime)
{
	if (_bWaitingForGrid)
	{
		if (!TryInitialize()) return;
	}

	Super::Tick(DeltaTime);
	if (_bIsCenteredOnTile)
	{
		if (_bHasFood)
		{
			_currentTile->AddPheromoneAmount( EPheromoneTypes::Food, PheromoneStrength);
			SeekHome();
		}
		else
		{
			_currentTile->AddPheromoneAmount(EPheromoneTypes::Home, PheromoneStrength);
			SeekFood();
		}
	}
	else
	{
		MoveToDestination(DeltaTime);

		if (_distanceTraveled >= _tileDistanceThreshold && _grid != nullptr)
		{
			GridTile* closestTile = _grid->GetClosestTile(GetActorLocation());

			if (*closestTile != *_currentTile)
			{
				_currentTile->Occupants.Remove(this);
				closestTile->Occupants.Add(this);
				_currentTile = closestTile;
			}
		}
	}
}

void AAnt::SeekFood()
{

}

void AAnt::PickUpFood()
{}

void AAnt::GiveFood()
{}

void AAnt::SeekHome()
{}

void AAnt::MoveToDestination(float deltaTime)
{
	FVector currentLocation = GetActorLocation();
	FVector newLocation = FMath::Lerp(currentLocation, CurrentDestination, deltaTime * MoveSpeed);

	if (FVector::Distance(newLocation, CurrentDestination) <= 1.0f)
	{
		// I think the unreal units are small, 1 should be around a cm I think 
		SetActorLocation(CurrentDestination);
		_bIsCenteredOnTile = true;
		return;
	}

	SetActorLocation(newLocation);
	_bIsCenteredOnTile = false;
}

