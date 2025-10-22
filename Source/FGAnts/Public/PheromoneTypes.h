// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "PheromoneTypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType, Blueprintable)
enum class EPheromoneTypes : uint8
{
	Home,
	Food,
	Danger,
	//EPheromoneTypes_Max UMETA(Hidden),
};
