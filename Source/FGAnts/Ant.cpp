// Fill out your copyright notice in the Description page of Project Settings.


#include "Ant.h"
#include "AntGrid.h"
#include "AntTile.h"

// Sets default values
AAnt::AAnt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAnt::SeekFood()
{}

void AAnt::SeekHome()
{}

void AAnt::MoveToDestination()
{}

