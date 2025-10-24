// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InitializeAfterGrid.h"
#include "Food.generated.h"

class USphereComponent;
class AAntGrid;
class GridTile;

UCLASS()
class FGANTS_API AFood : public AInitializeAfterGrid
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFood();

	UPROPERTY(EditAnywhere ,BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* DetectionSphere;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//int32 NumUsesInitial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumUsesRemaining{10};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool TryInitialize() override;

	//bool _bWaitingForGrid{ true };

	//AAntGrid* _grid{ nullptr };
	//GridTile* _currentTile{ nullptr };
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool PickUp();

};
