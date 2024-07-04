// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LaylaGameMode_FreeForAll.generated.h"

/**
 * 
 */
UCLASS()
class LAYLA_API ALaylaGameMode_FreeForAll : public AGameMode
{
	GENERATED_BODY()

public:

	ALaylaGameMode_FreeForAll();
	
	virtual void PreInitializeComponents() override;

	/** update remaining time */
	virtual void DefaultTimer();

	/** Tries to spawn the player's pawn */
	virtual void RestartPlayer(AController* NewPlayer) override;
	
	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;
	
	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** check who won */
	virtual void DetermineMatchWinner();

	/** check if PlayerState is a winner */
	virtual bool IsWinner(class ALaylaPlayerState* PlayerState) const ;

	/** best player */
	UPROPERTY(transient)
	class ALaylaPlayerState* WinnerPlayerState;

	
protected:
	virtual void BeginPlay() override;
	
	/** delay between first player login and starting match */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WarmupTime;

	/** match duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoundTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimeBetweenMatches;

	/** score for kill */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KillScore;

	/** score for death */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DeathScore;
	
private:

	TArray<AActor*> PlayerStarts;
	
	FTimerHandle TimerHandle_DefaultTimer;

	/** finish current match and lock players */
	UFUNCTION(exec)
	void FinishMatch();
};
