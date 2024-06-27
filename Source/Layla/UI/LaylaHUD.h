// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LaylaHUD.generated.h"

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
	void ToggleScoreBoardVisibility(bool bVisibility);
	void DrawGamePhase();
	void DrawScoreBoard();


	// Menus
	void DrawGameMenu();
	
protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> CharacterHUDClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> ScoreBoardItemClass;
	
private:
	
	UUserWidget* CharacterHUD;
	//
	// UUserWidget* ScoreBoardItem;
};
