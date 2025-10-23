// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PheromoneTypes.h"
#include "InitializeAfterGrid.h"
#include "Ant.generated.h"

class AAntGrid;
class GridTile;
class USphereComponent;

UCLASS()
class FGANTS_API AAnt : public AActor/*, public IInitializeAfterGrid*/
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnt();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PheromoneStrength{ 1.0f };

	UFUNCTION(BlueprintGetter)
	bool HasFood() const { return _bHasFood; }

	UFUNCTION(BlueprintGetter)
	bool IsCenteredOnTile() const { return _bIsCenteredOnTile; }

	UFUNCTION(BlueprintGetter)
	AAntGrid* GetGrid() const { return _grid; }

	/*UFUNCTION(BlueprintGetter)
	GridTile* GetCurrentTile() const;*/

	UPROPERTY(BlueprintReadWrite)
	FVector CurrentDestination{ 0, 0, 0 };

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* AntMesh{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* InteractionSphere;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool TryInitialize();
	
	bool _bWaitingForGrid{ true };

	UPROPERTY(BlueprintGetter = "HasFood")
	bool _bHasFood{ false };
	
	UPROPERTY(BlueprintGetter = "IsCenteredOnTile")
	bool _bIsCenteredOnTile{ false };

	UPROPERTY(BlueprintGetter = "GetGrid"/*, BlueprintSetter = "SetGrid"*/)
	AAntGrid* _grid{ nullptr };
	
	GridTile* _currentTile{ nullptr };

	float _distanceTraveled{ 0 };

	float _tileDistanceThreshold{ 25 };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetNextHighDestination(EPheromoneTypes typeToSeek, EPheromoneTypes typeToAvoid);
	void SetNextLowDestination(EPheromoneTypes typeToSeek, EPheromoneTypes typeToAvoid);
	void SetNextDestinationRandom(TArray<GridTile*>& neighbors);

	UFUNCTION(BlueprintCallable)
	bool SeekFood();

	UFUNCTION(BlueprintCallable)
	void PickUpFood();

	UFUNCTION(BlueprintCallable)
	void GiveFood();

	UFUNCTION(BlueprintCallable)
	bool SeekHome();

	UFUNCTION(BlueprintCallable)
	void MoveToDestination(float deltaTime);

	//virtual void InitializeWithGrid(AAntGrid* antGrid) override;
};
