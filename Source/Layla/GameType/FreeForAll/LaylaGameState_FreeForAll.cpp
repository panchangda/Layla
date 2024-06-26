// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGameState_FreeForAll.h"

#include "LaylaPlayerState_FreeForAll.h"
#include "Net/UnrealNetwork.h"

void ALaylaGameState_FreeForAll::GetRankedMap(RankedPlayerMap& OutRankedMap) const
{
	OutRankedMap.Empty();

	//first, we need to go over all the PlayerStates, grab their score, and rank them
	TMultiMap<int32, ALaylaPlayerState_FreeForAll*> SortedMap;
	for(int32 i = 0; i < PlayerArray.Num(); ++i)
	{
		int32 Score = 0;
		ALaylaPlayerState_FreeForAll* CurPlayerState = Cast<ALaylaPlayerState_FreeForAll>(PlayerArray[i]);
		if (CurPlayerState)
		{
			SortedMap.Add(FMath::TruncToInt(CurPlayerState->GetScore()), CurPlayerState);
		}
	}

	//sort by the keys
	SortedMap.KeySort(TGreater<int32>());

	//now, add them back to the ranked map
	OutRankedMap.Empty();

	int32 Rank = 0;
	for(TMultiMap<int32, ALaylaPlayerState_FreeForAll*>::TIterator It(SortedMap); It; ++It)
	{
		OutRankedMap.Add(Rank++, It.Value());
	}
}

ALaylaGameState_FreeForAll::ALaylaGameState_FreeForAll()
{
	NumPlayers = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}


void ALaylaGameState_FreeForAll::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( ALaylaGameState_FreeForAll, NumPlayers );
	DOREPLIFETIME( ALaylaGameState_FreeForAll, RemainingTime );
	DOREPLIFETIME( ALaylaGameState_FreeForAll, bTimerPaused );
}


