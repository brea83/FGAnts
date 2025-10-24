// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"
#include "Components/SphereComponent.h"
#include "AntGameState.h"
#include "AntGrid.h"
#include "GridTile.h"

// Sets default values
AFood::AFood()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Root Mesh"));
	RootComponent = Mesh;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Detection Range"));
	DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
	DetectionSphere->InitSphereRadius(80.0f);
}

// Called when the game starts or when spawned
void AFood::BeginPlay()
{
	Super::BeginPlay();

}


bool AFood::TryInitialize()
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

// Called every frame
void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (_bWaitingForGrid)
	{
		if (!TryInitialize()) return;
	}
	
	float foodStrength = _currentTile->GetPheromoneAmount(EPheromoneTypes::Food);
	if (foodStrength < 2)
	{
		_currentTile->AddPheromoneAmount(EPheromoneTypes::Food, DeltaTime);
	}

}

bool AFood::PickUp()
{

	if (NumUsesRemaining <= 0)
	{
		_currentTile->Occupants.Remove(this);
		_currentTile->ResetPheromone(EPheromoneTypes::Food);
		Destroy();
		return false;
	}

	NumUsesRemaining--;
	return true;
}

