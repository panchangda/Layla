// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGameMode_FreeForAll.h"

#include "EngineUtils.h"
#include "LaylaGameState_FreeForAll.h"
#include "LaylaPlayerState_FreeForAll.h"
#include "GameFramework/PlayerStart.h"
#include "UI/LaylaHUD.h"


void ALaylaGameMode_FreeForAll::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if(bDelayedStart)
	{
		// start warmup if needed
		ALaylaGameState_FreeForAll* const MyGameState = Cast<ALaylaGameState_FreeForAll>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
			if (bWantsMatchWarmup && WarmupTime > 0)
			{
				MyGameState->RemainingTime = WarmupTime;
			}
			else
			{
				MyGameState->RemainingTime = 0.0f;
			}
		}
	}

}

void ALaylaGameMode_FreeForAll::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	// Set Roundtime to GameState
	ALaylaGameState_FreeForAll* const MyGameState = Cast<ALaylaGameState_FreeForAll>(GameState);
	MyGameState->RemainingTime = RoundTime;	
}


ALaylaGameMode_FreeForAll::ALaylaGameMode_FreeForAll()
{
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = ALaylaHUD::StaticClass();
	PlayerStateClass = ALaylaPlayerState_FreeForAll::StaticClass();
	GameStateClass = ALaylaGameState_FreeForAll::StaticClass();
}

void ALaylaGameMode_FreeForAll::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	//
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ALaylaGameMode_FreeForAll::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);

}

void ALaylaGameMode_FreeForAll::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn,
	const UDamageType* DamageType)
{
	ALaylaPlayerState_FreeForAll* KillerPlayerState = Killer ? Cast<ALaylaPlayerState_FreeForAll>(Killer->PlayerState) : NULL;
	ALaylaPlayerState_FreeForAll* VictimPlayerState = KilledPlayer ? Cast<ALaylaPlayerState_FreeForAll>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
	}
}

AActor* ALaylaGameMode_FreeForAll::ChoosePlayerStart_Implementation(AController* Player)
{
	if (PlayerStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
		return PlayerStarts[RandomIndex];
	}

	// 默认行为
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ALaylaGameMode_FreeForAll::BeginPlay()
{
	Super::BeginPlay();

	// 获取所有PlayerStart
	for (TActorIterator<AActor> It(GetWorld(), APlayerStart::StaticClass()); It; ++It)
	{
		PlayerStarts.Add(*It);
	}
}

void ALaylaGameMode_FreeForAll::FinishMatch()
{
	ALaylaGameState_FreeForAll* const MyGameState = Cast<ALaylaGameState_FreeForAll>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
	}
	
}


void ALaylaGameMode_FreeForAll::DefaultTimer()
{
	ALaylaGameState_FreeForAll* const MyGameState = Cast<ALaylaGameState_FreeForAll>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;
		if(MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				RestartGame();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();

				// Send end round events
				// for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
				// {
				// 	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*It);
				// 	
				// 	if (PlayerController && MyGameState)
				// 	{
				// 		AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>((*It)->PlayerState);
				// 		const bool bIsWinner = IsWinner(PlayerState);
				// 	
				// 		PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
				// 	}
				// }
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
			}
		}
	}
}
