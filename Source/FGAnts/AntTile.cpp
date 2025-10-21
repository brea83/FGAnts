// Fill out your copyright notice in the Description page of Project Settings.


#include "AntTile.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"


AAntTile::AAntTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DebugMesh = CreateDefaultSubobject<UBoxComponent>(FName("Debug Mesh"));
	DebugMesh->SetRelativeLocation(FVector(0, 0, 0));
	DebugMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugMesh->InitBoxExtent(FVector(halfWidth, halfHeight, 5));

	 
}


//void AAntTile::InitializeTile(FVector cenerPoint)
//{}

void AAntTile::BeginPlay()
{
	Super::BeginPlay();

	bool bHitFound{ false };
	FHitResult outHit;
	FVector start = GetActorLocation();
	FVector end = start + (GetActorUpVector() * 100);
	FVector halfSize{ halfWidth, halfHeight, 10 };
	FRotator orientation = GetActorRotation();
	//ECollisionChannel blockersChannel = ECollisionChannel::ECC_WorldStatic;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	bHitFound = UKismetSystemLibrary::BoxTraceSingleByProfile(
		GetWorld(),
		start,
		end,
		halfSize,
		orientation,
		FName("BlockAll"),
		false,
		actorsToIgnore,
		EDrawDebugTrace::Persistent,
		outHit,
		true
		);

	FColor debugColor = FColor::Green;
	if (bHitFound)
	{
		bIsTraversable = false;
		debugColor = FColor::Red;
	}

	//DrawDebugBox(GetWorld(), GetActorLocation(), FVector(halfWidth, halfHeight, 5), debugColor, true, -1, 0, 10);
}

void AAntTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasPheromones)
	{
		TArray<FName> keys;
		Pheromones.GenerateKeyArray(keys);
		for (FName key : keys)
		{
			float newValue = *Pheromones.Find(key) - DeltaTime;

			if (newValue <= 0)
			{
				Pheromones.Remove(key);
			}
			else
			{
				Pheromones.Add(key, newValue);
			}
		}

		if (Pheromones.Num() <= 0)
		{
			bHasPheromones = false;
		}
	}
}

FVector AAntTile::GetUpperBounds()
{
	FVector centerPoint = GetActorLocation();
	return FVector(centerPoint.X + halfWidth, centerPoint.Y + halfHeight, centerPoint.Z);
}

FVector AAntTile::GetLowerBounds()
{
	FVector centerPoint = GetActorLocation();
	return FVector(centerPoint.X - halfWidth, centerPoint.Y - halfHeight, centerPoint.Z);
}

float AAntTile::GetPheromoneAmount(FName pheromoneName)
{
	float* pheromoneAmount = Pheromones.Find(pheromoneName);

	if (pheromoneAmount) return *pheromoneAmount;

	// if pheromoneAmount was an invalid pointer return a negative number,
	// functions calling this need to know and check for that a negative number is an invalid result
	return -1.0f;
}

void AAntTile::AddToPheromones(FName pheromoneName, float amount)
{
	if (amount <= 0) return; // this is not to be used for decrementing pheromone amounts

	float* oldAmount = Pheromones.Find(pheromoneName);

	if (oldAmount)
	{
		amount += *oldAmount;
	}

	Pheromones.Add(pheromoneName, amount);
	bHasPheromones = true;
}
