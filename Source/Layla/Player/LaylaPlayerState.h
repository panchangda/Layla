// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LaylaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LAYLA_API ALaylaPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void Reset() override;

	/** player killed someone */
	void ScoreKill(ALaylaPlayerState* Victim, int32 Points);

	/** player died */
	void ScoreDeath(ALaylaPlayerState* KilledBy, int32 Points);

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class ALaylaPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALaylaPlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class ALaylaPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALaylaPlayerState* KilledPlayerState);

	/** gets truncated player name to fit in death log and scoreboards */
	FString GetShortPlayerName() const;
	
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
