// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LaylaHUD.generated.h"

class ALaylaPlayerState;

USTRUCT()
struct FDeathMessage
{
	GENERATED_BODY()

	/** Name of player scoring kill. */
	FString KillerDesc;

	/** Name of killed player. */
	FString VictimDesc;

	/** Killer is local player. */
	uint8 bKillerIsOwner : 1;
	
	/** Victim is local player. */
	uint8 bVictimIsOwner : 1;

	/** Team number of the killer. */
	int32 KillerTeamNum;

	/** Team number of the victim. */
	int32 VictimTeamNum; 

	/** timestamp for removing message */
	float HideTime;

	/** What killed the player. */
	TWeakObjectPtr<class UDamageType> DamageType;

	/** Initialise defaults. */
	FDeathMessage()
		: bKillerIsOwner(false)
		, bVictimIsOwner(false)
		, KillerTeamNum(0)
		, VictimTeamNum(0)		
		, HideTime(0.f)
	{
	}
};
/**
 * 
 */
UCLASS()
class LAYLA_API ALaylaHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALaylaHUD();
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;


	// Game Infos
	void DrawAmmoHUD();
	void DrawRemainingTime();
	void DrawGamePhase();
	void DrawScoreBoard();
	void DrawHitIndicator();
	void ShowDeathMessage(ALaylaPlayerState* KillerPlayerState,
	ALaylaPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);
	void DrawDeathMessages();
	void DrawKillOrKilled();
	
	// Menus
	void DrawGameMenu();

	// Visibility Helpers: use by Player Controller
	void ToggleScoreBoardVisibility(bool bVisibility);
	void ShowScoreBoard();
	void HideScoreBoard();

	void ToggleGameMenuVisibility();

	
	void ToggleWinVisibility(bool bVisible = false);
	void ToggleLoseVisibility(bool bVisible = false);
	
protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> CharacterHUDClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ScoreBoardClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ScoreBoardItemClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> GameMenuClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DeathMessageItemClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryClass;

	
private:
	
	UUserWidget* CharacterHUD;
	
	UUserWidget* ScoreBoard;
	
	UUserWidget* GameMenu;

	UUserWidget* Inventory;
	//
	// UUserWidget* ScoreBoardItem;

	bool bScoreBoardVisible = false;
	bool bGameMenuVisible = false;

	bool bKillOrKilledVisible = false;
	
	FTimerHandle TimerHandle_KillOrKilled;

	void ShowKillOrKilled();
	void HideKillOrKilled();
	FString KillOrKilledMessage;
	
	TArray<FDeathMessage>DeathMessages;
};
