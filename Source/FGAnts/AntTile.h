// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntTile.generated.h"

class UBoxComponent;

UCLASS()
class FGANTS_API AAntTile : public AActor
{
	GENERATED_BODY()

public:
	AAntTile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float halfWidth{ 50 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float halfHeight{ 50 };

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> Occupants;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, float> Pheromones;


protected:
	UBoxComponent* DebugMesh{ nullptr };
	bool bHasPheromones{ false };
	bool bIsTraversable{ true };

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// this will need to query for occupants and blockers in the bounds

	//void InitializeTile(FVector cenerPoint);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector GetUpperBounds();

	UFUNCTION(BlueprintCallable)
	FVector GetLowerBounds();

	UFUNCTION(BlueprintCallable)
	float GetPheromoneAmount(FName pheromoneName);

	UFUNCTION(BlueprintCallable)
	void AddToPheromones(FName pheromoneName, float amount);
};
