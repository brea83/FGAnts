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
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		_remainingDistance = _maxMoves *( (_grid->TileSize.X + _grid->TileSize.Y)/2);

		_currentTile->Occupants.Add(this);
		CurrentDestination = _currentTile->GetCenterPosition();
		_traversedTiles.AddUnique(_currentTile);
		_bWaitingForGrid = false;
		return true;
	}

	return false;
	
}

void AAnt::DepositPheromone(EPheromoneTypes pheromone)
{
	float depositedPheromone{ 0 };

	switch (pheromone)
	{
	case EPheromoneTypes::Home:
		depositedPheromone = PheromoneStrength / _traversedTiles.Num();
		break;
	case EPheromoneTypes::Food:
		depositedPheromone = PheromoneStrength / _PathLengthToFood;
		break;
	case EPheromoneTypes::Danger:
		depositedPheromone = PheromoneStrength;
		break;
	default:
		break;
	}
	
	_currentTile->AddPheromoneAmount(pheromone, depositedPheromone);
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
				DepositPheromone(EPheromoneTypes::Home);
				_remainingDistance = _maxMoves * ((_grid->TileSize.X + _grid->TileSize.Y) / 2);
			}
			else
			{
				DepositPheromone(EPheromoneTypes::Food);
			}
			_bIsCenteredOnTile = false;
		}
		else if(_remainingDistance < 1 * ((_grid->TileSize.X + _grid->TileSize.Y) / 2))
		{ 
			bool foundHome = SeekHome();
			if (foundHome)
			{
				DepositPheromone(EPheromoneTypes::Home);
				_remainingDistance = _maxMoves * ((_grid->TileSize.X + _grid->TileSize.Y) / 2);
			}

			_bIsCenteredOnTile = false;
		}
		else
		{
			bool foundFood = SeekFood();
			if (foundFood)
			{
				
				_PathLengthToFood = FMath::Max<int>(1, _traversedTiles.Num());
				 DepositPheromone(EPheromoneTypes::Food);
			}
			else
			{
				 DepositPheromone(EPheromoneTypes::Home);
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
				_traversedTiles.AddUnique(_currentTile);
			}
		}
	}
}

void AAnt::SetNextDestination(EPheromoneTypes typeToSeek, EPheromoneTypes typeToAvoid, bool bCompareToTraversedTile)
{
	TArray<GridTile*> neighbors = _grid->GetNeighborTiles(_currentTile);

	if (neighbors.Num() <= 0) return;

	GridTile* highestSoughtTile;
	float highestSoughtAmount;
	if (bCompareToTraversedTile && !( _traversedTiles.IsEmpty() ) )
	{
		highestSoughtTile = _traversedTiles.Pop();
		highestSoughtAmount = highestSoughtTile->GetPheromoneAmount(typeToSeek);
	}
	else
	{
		highestSoughtTile = nullptr;
		highestSoughtAmount =  0.0f ;
	}


	GridTile* lowestAvoidTile{ nullptr };
	float lowestAvoidAmount{ FLT_MAX };

	float desirabilityRatio{ 0 };
	TArray<GridTile*> validNeighbors;

	for (GridTile* tile : neighbors)
	{
		//remove already visited neighbors.
		if (_traversedTiles.Contains(tile) || !tile->IsTraversable())
		{
			continue;
		}
		validNeighbors.Add(tile);

		// generate a random amount of curiosity interest in each neighbor to add to the desirability ratio
		// this needs to be small enough to not Always sway ants onto a random path
		float curiosityModifier = FMath::RandRange(0.0f, 1.2f);

		float currentSoughtAmount = tile->GetPheromoneAmount(typeToSeek);
		float currentAvoidAmount = tile->GetPheromoneAmount(typeToAvoid);
		desirabilityRatio = currentAvoidAmount > 0 ? currentSoughtAmount / currentAvoidAmount : currentSoughtAmount;

		desirabilityRatio *= curiosityModifier;

		float distanceWeight = FVector::DistSquared(GetActorLocation(), tile->GetCenterPosition()) * 0.0001f;

		desirabilityRatio /= distanceWeight;

		if (desirabilityRatio > highestSoughtAmount)
		{
			highestSoughtAmount = desirabilityRatio;
			highestSoughtTile = tile;
		}

		if (currentAvoidAmount <= lowestAvoidAmount && currentAvoidAmount > 0)
		{
			lowestAvoidAmount = currentAvoidAmount;
			lowestAvoidTile = tile;
		}

	}

	if (highestSoughtTile != nullptr)
	{
		if (GEngine && PrintDebug)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::Printf(TEXT("Desireability: %f"), highestSoughtAmount/*, distanceWeight*/));
		}
		CurrentDestination = highestSoughtTile->GetCenterPosition();
		return;
	}

	if (lowestAvoidTile != nullptr)
	{
		CurrentDestination = lowestAvoidTile->GetCenterPosition();
		return;
	}

	// if no tiles have pheromone data get a random neighbor
	SetNextDestinationRandom(validNeighbors);
}

void AAnt::SetNextDestinationRandom(TArray<GridTile*>& neighbors, bool doubleCheckTraversable)
{
	if (neighbors.IsEmpty())
	{
		// no valid neighbors, set pheromones empty and return home
		_remainingDistance = 0;
		if (_traversedTiles.IsEmpty()) return;

		CurrentDestination = _traversedTiles.Pop()->GetCenterPosition();
		return;
	}

	if (doubleCheckTraversable)
	{
		TArray<GridTile*> validNeighbors;

		for (GridTile* tile : neighbors)
		{
			if (!tile->IsTraversable())
			{
				continue;
			}
			validNeighbors.Add(tile);
		}

		int random = FMath::RandHelper(validNeighbors.Num() - 1);
		if (validNeighbors.IsValidIndex(random))
		{
			CurrentDestination = validNeighbors[random]->GetCenterPosition();
			return;
		}
	}
	else
	{
		int random = FMath::RandHelper(neighbors.Num() - 1);
		if (neighbors.IsValidIndex(random))
		{
			CurrentDestination = neighbors[random]->GetCenterPosition();
			return;
		}
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
		SetNextDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger);
		return false;
	}
	
	bool gotFood = foundFood->PickUp();

	if (!gotFood)
	{
		SetNextDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger);
		return false;
	}

	PickUpFood();
	 
	SetNextDestination(EPheromoneTypes::Home, EPheromoneTypes::Danger);
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

	if (homeFound != nullptr)
	{
		//tell home to do thing
		GiveFood();
		SetNextDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger);

		return true;
		
	}

	if (_traversedTiles.IsEmpty())
	{
		TArray<GridTile*> neighbors = _grid->GetNeighborTiles(_currentTile);
		SetNextDestinationRandom(neighbors, true);
		return false;
	}
	
	//SetNextDestination(EPheromoneTypes::Food, EPheromoneTypes::Danger, true);

	// traverse back along own path which is an array that doesn't decay, representing a unique pheromone per ant
	CurrentDestination = _traversedTiles.Pop()->GetCenterPosition();

	return false;
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
	float moveDistance = FVector::Distance(currentLocation, newLocation);
	_distanceTraveled += moveDistance;
	_remainingDistance -= moveDistance;
	_bIsCenteredOnTile = false;
}