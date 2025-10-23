// Fill out your copyright notice in the Description page of Project Settings.


#include "InitializeAfterGrid.h"
#include "AntGameState.h"
#include "../AntGrid.h"
#include "GridTile.h"

// Add default functionality here for any IInitializeAfterGrid functions that are not pure virtual.

AInitializeAfterGrid::AInitializeAfterGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInitializeAfterGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AInitializeAfterGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AInitializeAfterGrid::TryInitialize()
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
		_bWaitingForGrid = false;
		return true;
	}

	return false;

}