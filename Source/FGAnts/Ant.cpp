// Fill out your copyright notice in the Description page of Project Settings.


#include "Ant.h"
#include "AntGrid.h"
#include "GridTile.h"
#include "AntGameState.h"
#include "Components/SphereComponent.h"
#include "Food.h"
#include "AntHome.h"

// Sets default values
AAnt::AAnt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Range"));
	InteractionSphere->SetCollisionProfileName(TEXT("Pawn"));
	InteractionSphere->InitSphereRadius(40.0f);
}

// Called when the game starts or when spawned
void AAnt::BeginPlay()
{
	Super::BeginPlay();
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
	Super::Tick(DeltaTime);

	if (_bWaitingForGrid)
	{
		if (!TryInitialize()) return;
	}

	Super::Tick(DeltaTime);
	if (_bIsCenteredOnTile)
	{
		if (_bHasFood)
		{
			bool foundHome = SeekHome();
			if (foundHome)
			{
				_currentTile->AddPheromoneAmount(EPheromoneTypes::Home, PheromoneStrength);
			}
			else
			{
				_currentTile->AddPheromoneAmount(EPheromoneTypes::Food, PheromoneStrength);
			}
			_bIsCenteredOnTile = false;
		}
		else
		{
			bool foundFood = SeekFood();
			if (foundFood)
			{
				_currentTile->AddPheromoneAmount(EPheromoneTypes::Food, PheromoneStrength);
			}
			else
			{
				_currentTile->AddPheromoneAmount(EPheromoneTypes::Home, PheromoneStrength);
			}
			_bIsCenteredOnTile = false;
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

void AAnt::SetNextHighDestination(EPheromoneTypes typeToSeek, EPheromoneTypes typeToAvoid)
{
	TArray<GridTile*> neighbors = _grid->GetNeighborTiles(_currentTile);

	if (neighbors.Num() <= 0) return;

	GridTile* highestSoughtTile{ nullptr };
	float highestSoughtAmount{ 0 };

	GridTile* lowestAvoidTile{ nullptr };
	float lowestAvoidAmount{ FLT_MAX };

	for (GridTile* tile : neighbors)
	{
		float currentSoughtAmount = tile->GetPheromoneAmount(typeToSeek);
		if (currentSoughtAmount >= highestSoughtAmount)
		{
			highestSoughtAmount = currentSoughtAmount;
			highestSoughtTile = tile;
		}

		float currentAvoidAmount = tile->GetPheromoneAmount(typeToAvoid);
		if (currentAvoidAmount <= lowestAvoidAmount && currentAvoidAmount > 0)
		{
			lowestAvoidAmount = currentAvoidAmount;
			lowestAvoidTile = tile;
		}

	}

	if (highestSoughtTile != nullptr)
	{
		CurrentDestination = highestSoughtTile->GetCenterPosition();
		return;
	}

	if (lowestAvoidTile != nullptr)
	{
		CurrentDestination = lowestAvoidTile->GetCenterPosition();
		return;
	}

	// if no tiles have pheromone data get a random neighbor
	SetNextDestinationRandom(neighbors);
}

void AAnt::SetNextLowDestination(EPheromoneTypes typeToSeek, EPheromoneTypes typeToAvoid)
{
	TArray<GridTile*> neighbors = _grid->GetNeighborTiles(_currentTile);

	if (neighbors.Num() <= 0) return;

	GridTile* lowestSoughtTile{ nullptr };
	float lowestSoughtAmount{ FLT_MAX };

	GridTile* lowestAvoidTile{ nullptr };
	float lowestAvoidAmount{ FLT_MAX };

	for (GridTile* tile : neighbors)
	{
		float currentSoughtAmount = tile->GetPheromoneAmount(typeToSeek);
		if (currentSoughtAmount <= lowestSoughtAmount && currentSoughtAmount > 0)
		{
			lowestSoughtAmount = currentSoughtAmount;
			lowestSoughtTile = tile;
		}

		float currentAvoidAmount = tile->GetPheromoneAmount(typeToAvoid);
		if (currentAvoidAmount <= lowestAvoidAmount && currentAvoidAmount > 0)
		{
			lowestAvoidAmount = currentAvoidAmount;
			lowestAvoidTile = tile;
		}

	}

	if (lowestSoughtTile != nullptr)
	{
		CurrentDestination = lowestSoughtTile->GetCenterPosition();
		return;
	}

	if (lowestAvoidTile != nullptr)
	{
		CurrentDestination = lowestAvoidTile->GetCenterPosition();
		return;
	}

	// if no tiles have pheromone data get a random neighbor
	SetNextDestinationRandom(neighbors);
}

void AAnt::SetNextDestinationRandom(TArray<GridTile*>& neighbors)
{
	int random = FMath::RandHelper(neighbors.Num() - 1);
	if (neighbors.IsValidIndex(random))
	{
		CurrentDestination = neighbors[random]->GetCenterPosition();
		return;
	}
	CurrentDestination = neighbors[0]->GetCenterPosition();
}

bool AAnt::SeekFood()
{
	AFood* foundFood{ nullptr };

	for (AActor* actor : _currentTile->Occupants)
	{
		AFood* maybeFood = Cast<AFood>(actor);
		if (maybeFood != nullptr)
		{
			foundFood = maybeFood;
			break;
		}
	}

	if (foundFood == nullptr)
	{
		SetNextHighDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger);
		return false;
	}
	
	foundFood->PickUp();
	PickUpFood();
	SetNextLowDestination(EPheromoneTypes::Home, EPheromoneTypes::Danger);
	return true;
}

void AAnt::PickUpFood()
{
	_bHasFood = true;
}

void AAnt::GiveFood()
{
	_bHasFood = false;
}

bool AAnt::SeekHome()
{
	AAntHome* homeFound{ nullptr };

	for (AActor* actor : _currentTile->Occupants)
	{
		AAntHome* maybeHome = Cast<AAntHome>(actor);
		if (maybeHome != nullptr)
		{
			homeFound = maybeHome;
			break;
		}
	}

	if (homeFound == nullptr)
	{
		SetNextHighDestination(EPheromoneTypes::Home, EPheromoneTypes::Danger);
		return false;
	}

	//tell home to do thing
	GiveFood();
	SetNextHighDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger);
	return true;
}

void AAnt::MoveToDestination(float deltaTime)
{
	FVector currentLocation = GetActorLocation();
	FVector newLocation = FMath::Lerp(currentLocation, CurrentDestination, deltaTime * MoveSpeed);


	if (FVector::Distance(newLocation, CurrentDestination) <= 1.0f)
	{
		// I think the unreal units are small, 1 should be around a cm I think 
		SetActorLocation(CurrentDestination);
		_bIsCenteredOnTile = true;
		_distanceTraveled = 0.0f;
		return;
	}

	SetActorLocation(newLocation);
	_distanceTraveled += FVector::Distance(currentLocation, newLocation);
	_bIsCenteredOnTile = false;
}
//void AAnt::InitializeWithGrid(AAntGrid* antGrid)
//{
//	if (antGrid == nullptr)
//	{
//		_bWaitingForGrid = true;
//		return;
//	}
//
//	_grid = antGrid;
//
//	_currentTile = _grid->GetClosestTile(GetActorLocation());
//
//	if (_currentTile != nullptr)
//	{
//		_currentTile->Occupants.Add(this);
//		CurrentDestination = _currentTile->GetCenterPosition();
//		_bWaitingForGrid = false;
//		return;
//	}
//
//	_bWaitingForGrid = true;
//}
//
//void AAnt::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	AFood* food = Cast<AFood>(OtherActor);
//}

