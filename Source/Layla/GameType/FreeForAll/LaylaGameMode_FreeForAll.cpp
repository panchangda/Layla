// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGameMode_FreeForAll.h"

#include "EngineUtils.h"
#include "LaylaGameState_FreeForAll.h"
#include "GameFramework/PlayerStart.h"
#include "Player/LaylaCharacter.h"
#include "Player/LaylaPlayerController.h"
#include "Player/LaylaPlayerState.h"
#include "UI/LaylaHUD.h"
#include "Weapon/LaylaGun.h"


void ALaylaGameMode_FreeForAll::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if(bDelayedStart)
	{
		// start warmup if needed
		ALaylaGameState_FreeForAll* const MyGameState = Cast<ALaylaGameState_FreeForAll>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			// const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
			const bool bWantsMatchWarmup = true;
			if (bWantsMatchWarmup && WarmupTime > 0)
			{
				MyGameState->RemainingTime = WarmupTime;
				MyGameState->GamePhase = EGamePhase_FreeForAll::WarmUp;
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
	MyGameState->GamePhase = EGamePhase_FreeForAll::InGame;

	// notify players
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ALaylaPlayerController* PC = Cast<ALaylaPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

ALaylaGameMode_FreeForAll::ALaylaGameMode_FreeForAll()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/B_LaylaCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = ALaylaHUD::StaticClass();
	PlayerStateClass = ALaylaPlayerState::StaticClass();
	GameStateClass = ALaylaGameState_FreeForAll::StaticClass();

	MinRespawnDelay = 1.5f;
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
	ALaylaPlayerState* KillerPlayerState = Killer ? Cast<ALaylaPlayerState>(Killer->PlayerState) : NULL;
	ALaylaPlayerState* VictimPlayerState = KilledPlayer ? Cast<ALaylaPlayerState>(KilledPlayer->PlayerState) : NULL;

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

void ALaylaGameMode_FreeForAll::DetermineMatchWinner()
{
	ALaylaGameState_FreeForAll const* const MyGameState = CastChecked<ALaylaGameState_FreeForAll>(GameState);
	float BestScore = MIN_flt;
	int32 BestPlayer = -1;
	int32 NumBestPlayers = 0;

	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++)
	{
		const float PlayerScore = MyGameState->PlayerArray[i]->GetScore();
		if (BestScore < PlayerScore)
		{
			BestScore = PlayerScore;
			BestPlayer = i;
			NumBestPlayers = 1;
		}
		else if (BestScore == PlayerScore)
		{
			NumBestPlayers++;
		}
	}

	WinnerPlayerState = (NumBestPlayers == 1) ? Cast<ALaylaPlayerState>(MyGameState->PlayerArray[BestPlayer]) : NULL;
}

bool ALaylaGameMode_FreeForAll::IsWinner(ALaylaPlayerState* PlayerState) const
{
	return PlayerState && PlayerState == WinnerPlayerState; // && !PlayerState->IsQuitter();
}

void ALaylaGameMode_FreeForAll::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Notify Client, Client may want to send messages before beginplay
	if(ALaylaPlayerController* LaylaPlayerController = Cast<ALaylaPlayerController>(NewPlayer))
	{
		LaylaPlayerController->ClientOnPostLogin();
	}
	
}

UClass* ALaylaGameMode_FreeForAll::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<ALaylaPlayerController>())
	{
		if(ALaylaPlayerState* LaylaPlayerState = Cast<ALaylaPlayerState>(InController->PlayerState))
		{
			return LaylaPlayerState->GetPlayerHero().CharacterBP;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
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
	ALaylaGameState_FreeForAll* const GameState_FreeForAll = Cast<ALaylaGameState_FreeForAll>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			ALaylaPlayerState* PlayerState = Cast<ALaylaPlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
			
		}
		
		GameState_FreeForAll->GamePhase = EGamePhase_FreeForAll::GameOver;

		// lock all pawns
		// pawns are not marked as keep for seamless travel, so we will create new pawns on the next match rather than
		// turning these back on.
		for (APawn* Pawn : TActorRange<APawn>(GetWorld()))
		{
			Pawn->TurnOff();
		}

		// set up to restart the match
		GameState_FreeForAll->RemainingTime = TimeBetweenMatches;
		
	}
	
}


void ALaylaGameMode_FreeForAll::DefaultTimer()
{
	// don't update timers for Play In Editor mode, it's not real match
	// if (GetWorld()->IsPlayInEditor())
	// {
	// 	// start match if necessary.
	// 	if (GetMatchState() == MatchState::WaitingToStart)
	// 	{
	// 		StartMatch();
	// 	}
	// 	return;
	// }
	
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

				//Send end round events
				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
				{
					ALaylaPlayerController* PlayerController = Cast<ALaylaPlayerController>(*It);
					
					if (PlayerController && MyGameState)
					{
						ALaylaPlayerState* PlayerState = Cast<ALaylaPlayerState>((*It)->PlayerState);
						const bool bIsWinner = IsWinner(PlayerState);
					
						PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
					}
				}
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
			}
		}
	}
}

void ALaylaGameMode_FreeForAll::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	ALaylaPlayerController* PC = Cast<ALaylaPlayerController>(NewPlayer);
	if (PC)
	{
		// Since initial weapon is equipped before the pawn is added to the replication graph, need to resend the notify so that it can be added as a dependent actor
		ALaylaCharacter* Character = Cast<ALaylaCharacter>(PC->GetCharacter());
		if (Character && Character->DefaultGun)
		{
			ALaylaGun* DefaultGun = GetWorld()->SpawnActor<ALaylaGun>(Character->DefaultGun);
			Character->AddGun(DefaultGun);
		}
		
		PC->ClientGameStarted();
	}
}
