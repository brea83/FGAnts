// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ant.generated.h"

UCLASS()
class FGANTS_API AAnt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnt();

	UPROPERTY(BlueprintReadWrite)
	float MoveSpeed{ 1.0f };

	UPROPERTY(BlueprintReadWrite)
	bool BHasFood{ false };
	
	UPROPERTY(BlueprintReadWrite)
	bool BIsCenteredOnTile{ false };

	UPROPERTY(BlueprintReadWrite)
	int ClosestGridTileIndex{ 0 }; // replace with pointer to actual tile for faster access

	UPROPERTY(BlueprintReadWrite)
	FVector CurrentDestination;

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* AntMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SeekFood();

	UFUNCTION(BlueprintCallable)
	void SeekHome();

	UFUNCTION(BlueprintCallable)
	void MoveToDestination();

};
