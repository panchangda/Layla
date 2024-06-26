// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LaylaGameState_FreeForAll.generated.h"

class ALaylaPlayerState_FreeForAll;

/**
 * 
 */
/** ranked PlayerState map, created from the GameState */
typedef TMap<int32, TWeakObjectPtr<ALaylaPlayerState_FreeForAll> > RankedPlayerMap;

UCLASS()
class LAYLA_API ALaylaGameState_FreeForAll : public AGameState
{
	GENERATED_BODY()

public:

	/** gets ranked PlayerState map for specific team */
	void GetRankedMap(RankedPlayerMap& OutRankedMap) const;
	
	ALaylaGameState_FreeForAll();
	
	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
	int32 NumPlayers;
	
	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;
};
