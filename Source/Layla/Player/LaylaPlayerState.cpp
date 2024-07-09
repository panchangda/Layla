// Fill out your copyright notice in the Description page of Project Settings.



#include "LaylaPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Player/LaylaPlayerController.h"
#include "System/LaylaGameInstance.h"

void ALaylaPlayerState::Reset()
{
	Super::Reset();


	NumKills = 0;
	NumDeaths = 0;
	bQuitter = false;
}

void ALaylaPlayerState::ScoreKill(ALaylaPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ALaylaPlayerState::ScoreDeath(ALaylaPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

FString ALaylaPlayerState::GetShortPlayerName() const
{
	constexpr int MAX_PLAYER_NAME_LENGTH = 12;
	
	if( GetPlayerName().Len() > MAX_PLAYER_NAME_LENGTH )
	{
		return GetPlayerName().Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}
	return GetPlayerName();
}

int32 ALaylaPlayerState::GetKill()
{
	return NumKills;
}

int32 ALaylaPlayerState::GetDeath()
{
	return  NumDeaths;
}

void ALaylaPlayerState::SetPlayerHero(FLaylaHeroStruct NewPlayerHero)
{
	if(HasAuthority())
	{
		PlayerHero = NewPlayerHero;	
	}
}

FLaylaHeroStruct ALaylaPlayerState::GetPlayerHero()
{
	return PlayerHero;
}


void ALaylaPlayerState::BroadcastDeath_Implementation(ALaylaPlayerState* KillerPlayerState,
                                                      const UDamageType* KillerDamageType, ALaylaPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		ALaylaPlayerController* TestPC = Cast<ALaylaPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);				
		}
	}	
	
}

void ALaylaPlayerState::InformAboutKill_Implementation(ALaylaPlayerState* KillerPlayerState,
                                                                  const UDamageType* KillerDamageType, ALaylaPlayerState* KilledPlayerState)
{
	
}

void ALaylaPlayerState::ScorePoints(int32 Points)
{
	SetScore(GetScore() + Points);
}

void ALaylaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALaylaPlayerState, NumKills);
	DOREPLIFETIME(ALaylaPlayerState, NumDeaths);
	DOREPLIFETIME(ALaylaPlayerState, MatchId);
	DOREPLIFETIME(ALaylaPlayerState, PlayerHero);
}
