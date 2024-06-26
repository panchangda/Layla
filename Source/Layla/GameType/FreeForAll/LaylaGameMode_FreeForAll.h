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

	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;
	
	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
protected:
	virtual void BeginPlay() override;
	
	/** delay between first player login and starting match */
	UPROPERTY(config)
	int32 WarmupTime;

	/** match duration */
	UPROPERTY(config)
	int32 RoundTime;
    	
	/** score for kill */
	UPROPERTY(config)
	int32 KillScore;

	/** score for death */
	UPROPERTY(config)
	int32 DeathScore;
	
private:

	TArray<AActor*> PlayerStarts;
	
	FTimerHandle TimerHandle_DefaultTimer;

	/** finish current match and lock players */
	UFUNCTION(exec)
	void FinishMatch();
};
