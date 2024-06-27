// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaHUD.h"

#include "LaylaGun.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameType/FreeForAll/LaylaGameState_FreeForAll.h"
#include "GameType/FreeForAll/LaylaPlayerState_FreeForAll.h"
#include "Player/LaylaCharacter.h"

ALaylaHUD::ALaylaHUD()
{
	// // set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUD(TEXT("/Game/UI/W_UserHud"));
	// if (PlayerHUD.Class != NULL)
	// {
	// 	CharacterHUDClass = PlayerHUD.Class;
	// }
}

void ALaylaHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawRemainingTime();
	DrawGamePhase();
	DrawScoreBoard();
}

void ALaylaHUD::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterHUDClass)
	{
		CharacterHUD = CreateWidget<UUserWidget>(GetWorld(), CharacterHUDClass);
		if (CharacterHUD)
		{
			CharacterHUD->AddToViewport();
		}
	}
}

// Implement in BP Binding Functions
void ALaylaHUD::DrawAmmoHUD()
{
	ALaylaCharacter* Character = Cast<ALaylaCharacter>(GetOwningPawn());
	if(Character && CharacterHUD)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CharacterHUD->WidgetTree->FindWidget("TextBlock_Ammo"));

		if(AmmoText)
		{
			if(Character->CurrentGun){
				int32 AmmoInMagzine = Character->CurrentGun->AmmoInMagzine;
				AmmoText->SetText(FText::AsNumber(AmmoInMagzine));
			}else
			{
				AmmoText->SetText(FText::GetEmpty());
			}
			
		}
	}
	
}

void ALaylaHUD::DrawRemainingTime()
{
	ALaylaGameState_FreeForAll* GameState_FreeForAll = Cast<ALaylaGameState_FreeForAll>(GetWorld()->GetGameState());
	if(GameState_FreeForAll && CharacterHUD)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CharacterHUD->WidgetTree->FindWidget("TextBlock_RemainTime"));
		if(AmmoText)
		{
			AmmoText->SetText(FText::AsNumber(GameState_FreeForAll->RemainingTime));
		}
	}
}

void ALaylaHUD::ToggleScoreBoardVisibility(bool bVisibility)
{
	
}

void ALaylaHUD::DrawGamePhase()
{
	ALaylaGameState_FreeForAll* GameState_FreeForAll = Cast<ALaylaGameState_FreeForAll>(GetWorld()->GetGameState());
	if(GameState_FreeForAll && CharacterHUD)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CharacterHUD->WidgetTree->FindWidget("TextBlock_GameStage"));
		if (AmmoText)
		{
			FText GamePhaseText;
			switch (GameState_FreeForAll->GamePhase)
			{
			case EGamePhase_FreeForAll::WarmUp:
				GamePhaseText = FText::FromString(TEXT("Warm Up"));break;
			case EGamePhase_FreeForAll::InGame:
				GamePhaseText = FText::FromString(TEXT("In Game"));break;
			case EGamePhase_FreeForAll::GameOver:
				GamePhaseText = FText::FromString(TEXT("Game Over"));break;
			case EGamePhase_FreeForAll::Pause:
				GamePhaseText = FText::FromString(TEXT("Pause"));break;
			default:
				GamePhaseText = FText::GetEmpty();
			}
			
			AmmoText->SetText(GamePhaseText);
		}
	}

}

void ALaylaHUD::DrawScoreBoard()
{
	ALaylaGameState_FreeForAll* GameState_FreeForAll = Cast<ALaylaGameState_FreeForAll>(GetWorld()->GetGameState());
	if(CharacterHUD && GameState_FreeForAll)
	{
		UVerticalBox* ScoreBoardVBox = Cast<UVerticalBox>(CharacterHUD->WidgetTree->FindWidget("VerticalBox_ScoreBoard"));
		if(ScoreBoardVBox)
		{
			ScoreBoardVBox->ClearChildren();
			RankedPlayerMap InRankPlayerMap;
			GameState_FreeForAll->GetRankedMap(InRankPlayerMap);
			for(auto Player: InRankPlayerMap)
			{
				// 例如，你可以在这里添加其他Widget
				UUserWidget* PlayerItem = CreateWidget<UUserWidget>(GetWorld(), ScoreBoardItemClass);
				if (PlayerItem)
				{
					UTextBlock* Name = Cast<UTextBlock>(PlayerItem->WidgetTree->FindWidget("TextBlock_Name"));
					UTextBlock* Kill = Cast<UTextBlock>(PlayerItem->WidgetTree->FindWidget("TextBlock_Kill"));
					UTextBlock* Death = Cast<UTextBlock>(PlayerItem->WidgetTree->FindWidget("TextBlock_Death"));
					UTextBlock* Score = Cast<UTextBlock>(PlayerItem->WidgetTree->FindWidget("TextBlock_Score"));
					UTextBlock* Ping = Cast<UTextBlock>(PlayerItem->WidgetTree->FindWidget("TextBlock_Ping"));
					Name->SetText(FText::FromString(Player.Value->GetHumanReadableName()));
					Kill->SetText(FText::AsNumber(Player.Value->GetKill()));
					Death->SetText(FText::AsNumber(Player.Value->GetDeath()));
					Score->SetText(FText::AsNumber(Player.Value->GetScore()));
					Ping->SetText(FText::AsNumber(Player.Value->GetPingInMilliseconds()));
					ScoreBoardVBox->AddChildToVerticalBox(PlayerItem);
				}
			}
		}
	}
}

void ALaylaHUD::DrawGameMenu()
{

}
