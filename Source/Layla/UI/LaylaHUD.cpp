// Fill out your copyright notice in the Description page of Project Settings.
#include "LaylaHUD.h"

#include "LaylaGun.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "GameType/FreeForAll/LaylaGameState_FreeForAll.h"
#include "Player/LaylaCharacter.h"
#include "Player/LaylaPlayerState.h"

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

	if(bScoreBoardVisible)
	{
		DrawScoreBoard();
	}

	if(bGameMenuVisible)
	{
		DrawGameMenu();
	}
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
	if(ScoreBoardClass)
	{
		ScoreBoard = CreateWidget<UUserWidget>(GetWorld(), ScoreBoardClass);
		if(ScoreBoard)
		{
			ScoreBoard->AddToViewport();
			ScoreBoard->SetVisibility(ESlateVisibility::Collapsed);

			// Hide Template Child in ScoreBoard
			UVerticalBox* ScoreBoardItemContainerVBox = Cast<UVerticalBox>(
				ScoreBoard->WidgetTree->FindWidget("VerticalBox_ScoreBoardItemContainer"));
			if (ScoreBoardItemContainerVBox)
			{
				ScoreBoardItemContainerVBox->ClearChildren();
			}
		}
	}
	if(GameMenuClass)
	{
		GameMenu = CreateWidget<UUserWidget>(GetWorld(), GameMenuClass);
		if (GameMenu)
		{
			GameMenu->AddToViewport();
			GameMenu->SetVisibility(ESlateVisibility::Collapsed);
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
	if(ScoreBoard && GameState_FreeForAll)
	{
		UVerticalBox* ScoreBoardItemContainerVBox = Cast<UVerticalBox>(ScoreBoard->WidgetTree->FindWidget("VerticalBox_ScoreBoardItemContainer"));
		if(ScoreBoardItemContainerVBox)
		{
			ScoreBoardItemContainerVBox->ClearChildren();
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
					ScoreBoardItemContainerVBox->AddChildToVerticalBox(PlayerItem);
				}
			}
		}
	}
}

void ALaylaHUD::DrawHitIndicator()
{
	
}

void ALaylaHUD::ShowDeathMessage(ALaylaPlayerState* KillerPlayerState,
	ALaylaPlayerState* VictimPlayerState, const UDamageType* KillerDamageType)
{
	const int32 MaxDeathMessages = 5;
	const float MessageDuration = 10.0f;
	if(GetWorld()->GetGameState())
	{
		ALaylaPlayerState* MyPlayerState = PlayerOwner ? Cast<ALaylaPlayerState>(PlayerOwner->PlayerState) : nullptr;

		if(KillerPlayerState && VictimPlayerState && MyPlayerState && KillerDamageType)
		{
			if(DeathMessages.Num() >= MaxDeathMessages)
			{
				DeathMessages.RemoveAt(0, 1, false);
			}
			FDeathMessage NewMessage;
			NewMessage.KillerDesc = KillerPlayerState->GetShortPlayerName();
			NewMessage.VictimDesc = VictimPlayerState->GetShortPlayerName();
			// NewMessage.KillerTeamNum = KillerPlayerState->GetTeamNum();
			// NewMessage.VictimTeamNum = VictimPlayerState->GetTeamNum();
			NewMessage.bKillerIsOwner = MyPlayerState == KillerPlayerState;
			NewMessage.bVictimIsOwner = MyPlayerState == VictimPlayerState;

			NewMessage.DamageType = MakeWeakObjectPtr(const_cast<UDamageType*>(Cast<const UDamageType>(KillerDamageType)));
			NewMessage.HideTime = GetWorld()->GetTimeSeconds() + MessageDuration;

			DeathMessages.Add(NewMessage);
			if (KillerPlayerState == MyPlayerState && VictimPlayerState != MyPlayerState)
			{
				// LastKillTime = GetWorld()->GetTimeSeconds();
				// CenteredKillMessage = FText::FromString(NewMessage.VictimDesc);
			}
		}
	}


}

void ALaylaHUD::DrawGameMenu()
{

}

void ALaylaHUD::ShowScoreBoard()
{
	bScoreBoardVisible = true;
	if(ScoreBoard)
	{
		ScoreBoard->SetVisibility(ESlateVisibility::Visible);
	}
}

void ALaylaHUD::HideScoreBoard()
{
	bScoreBoardVisible = false;
	if(ScoreBoard)
	{
		ScoreBoard->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ALaylaHUD::ToggleGameMenuVisibility()
{
	bGameMenuVisible = !bGameMenuVisible;
	if(GameMenu){
		if(bGameMenuVisible == true)
		{
			GameMenu->SetVisibility(ESlateVisibility::Visible);
		}else
		{
			GameMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

