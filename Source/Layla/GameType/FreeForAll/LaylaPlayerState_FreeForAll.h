// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LaylaPlayerState.h"
#include "LaylaPlayerState_FreeForAll.generated.h"

/**
 * 
 */
UCLASS()
class LAYLA_API ALaylaPlayerState_FreeForAll : public ALaylaPlayerState
{
	GENERATED_BODY()

public:
	virtual void Reset() override;

	/** player killed someone */
	void ScoreKill(ALaylaPlayerState_FreeForAll* Victim, int32 Points);

	/** player died */
	void ScoreDeath(ALaylaPlayerState_FreeForAll* KilledBy, int32 Points);

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class ALaylaPlayerState_FreeForAll* KillerPlayerState, const UDamageType* KillerDamageType, class ALaylaPlayerState_FreeForAll* KilledPlayerState);

	/** broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class ALaylaPlayerState_FreeForAll* KillerPlayerState, const UDamageType* KillerDamageType, class ALaylaPlayerState_FreeForAll* KilledPlayerState);

	int32 GetKill();
	int32 GetDeath();
protected:

	/** number of kills */
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/** number of deaths */
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/** whether the user quit the match */
	UPROPERTY()
	uint8 bQuitter : 1;

	/** Match id */
	UPROPERTY(Replicated)
	FString MatchId;

private:

	/** helper for scoring points */
	void ScorePoints(int32 Points);
};
