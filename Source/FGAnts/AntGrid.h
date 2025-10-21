// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntGrid.generated.h"

class AAntTile;

UCLASS()
class FGANTS_API AAntGrid : public AActor
{
	GENERATED_BODY()

public:
	AAntGrid();
	void InitializeGrid();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D GridSize{ 10, 10 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D TileSize{ 100, 100 };

	TArray<AAntTile*> Tiles;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AAntTile* GetClosestTile(FVector queryPosition);
	
};
