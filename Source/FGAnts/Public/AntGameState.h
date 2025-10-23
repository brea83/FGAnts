// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AntGameState.generated.h"

class AAntGrid;
/**
 * 
 */
UCLASS()
class FGANTS_API AAntGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	void SetActiveGrid(AAntGrid* grid) { _activeGrid = grid; }
	AAntGrid* GetActiveGrid() { return _activeGrid; }

protected:
	AAntGrid* _activeGrid{ nullptr };

};
