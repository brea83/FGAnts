// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InitializeAfterGrid.generated.h"

class USphereComponent;
class AAntGrid;
class GridTile;

/**
 * Interface for actors that need to wait for the ant grid 
 * before they can finish initializing their parameters and behavior
 */
UCLASS()
class FGANTS_API AInitializeAfterGrid : public AActor
{
	GENERATED_BODY()


public:
	AInitializeAfterGrid();

	virtual bool TryInitialize();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool _bWaitingForGrid{ true };

	AAntGrid* _grid{ nullptr };
	GridTile* _currentTile{ nullptr };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
