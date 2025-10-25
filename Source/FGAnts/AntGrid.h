// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntGrid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAntGrid, Log, All);

class GridTile;
struct NeighborCoords
{
	FIntVector2 N {  0,  1 };
	FIntVector2 W { -1,  0 };
	FIntVector2 E {  1,  0 };
	FIntVector2 S {  0, -1 };

	FIntVector2 NW{ -1,  1 };
	FIntVector2 NE{  1,  1 };
	FIntVector2 SW{ -1, -1 };
	FIntVector2 SE{  1, -1 };
};

UCLASS()
class FGANTS_API AAntGrid : public AActor
{
	GENERATED_BODY()

public:
	AAntGrid();
	void InitializeGrid();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ShowFoodDebug{ false };
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PheromoneDecaySpeed{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ShowHomeDebug{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D GridSize{ 10, 10 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D TileSize{ 100, 100 };

	TArray<GridTile*> Tiles;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsTileTraversable(FVector centerPosition);

	GridTile* GetTileByCoord(FIntVector2 coord);


	const NeighborCoords _neighborCoords;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	GridTile* GetClosestTile(FVector queryPosition);

	TArray<GridTile*> GetNeighborTiles(GridTile* currentTile);
	
};
