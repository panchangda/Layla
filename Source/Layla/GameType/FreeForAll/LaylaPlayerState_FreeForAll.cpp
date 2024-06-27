// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaPlayerState_FreeForAll.h"

#include "Net/UnrealNetwork.h"

void ALaylaPlayerState_FreeForAll::Reset()
{
	Super::Reset();


	NumKills = 0;
	NumDeaths = 0;
	bQuitter = false;
}

void ALaylaPlayerState_FreeForAll::ScoreKill(ALaylaPlayerState_FreeForAll* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ALaylaPlayerState_FreeForAll::ScoreDeath(ALaylaPlayerState_FreeForAll* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

int32 ALaylaPlayerState_FreeForAll::GetKill()
{
	return NumKills;
}

int32 ALaylaPlayerState_FreeForAll::GetDeath()
{
	return  NumDeaths;
}

void ALaylaPlayerState_FreeForAll::BroadcastDeath_Implementation(ALaylaPlayerState_FreeForAll* KillerPlayerState,
                                                                 const UDamageType* KillerDamageType, ALaylaPlayerState_FreeForAll* KilledPlayerState)
{
	
}

void ALaylaPlayerState_FreeForAll::InformAboutKill_Implementation(ALaylaPlayerState_FreeForAll* KillerPlayerState,
                                                                  const UDamageType* KillerDamageType, ALaylaPlayerState_FreeForAll* KilledPlayerState)
{
	
}

void ALaylaPlayerState_FreeForAll::ScorePoints(int32 Points)
{
	SetScore(GetScore() + Points);
}

void ALaylaPlayerState_FreeForAll::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALaylaPlayerState_FreeForAll, NumKills);
	DOREPLIFETIME(ALaylaPlayerState_FreeForAll, NumDeaths);
	DOREPLIFETIME(ALaylaPlayerState_FreeForAll, MatchId);
}
