// Fill out your copyright notice in the Description page of Project Settings.
#include "LaylaHUD.h"

#include "Weapon/LaylaGun.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "GameType/FreeForAll/LaylaGameState_FreeForAll.h"
#include "Player/LaylaCharacter.h"
#include "Player/LaylaPlayerState.h"

class UBorder;

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

	DrawDeathMessages();
	
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

	if (ScoreBoard && GameState_FreeForAll && ScoreBoardItemClass)
	{
		UVerticalBox* ScoreBoardItemContainerVBox = Cast<UVerticalBox>(
			ScoreBoard->WidgetTree->FindWidget("VerticalBox_ScoreBoardItemContainer"));
		if (ScoreBoardItemContainerVBox)
		{
			ScoreBoardItemContainerVBox->ClearChildren();
			RankedPlayerMap InRankPlayerMap;
			GameState_FreeForAll->GetRankedMap(InRankPlayerMap);
			for (auto Player : InRankPlayerMap)
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
	const int32 MaxDeathMessages = 4;
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
				KillOrKilledMessage = FString::Printf(TEXT("You Killed %s, Using %s"), *VictimPlayerState->GetShortPlayerName(), "Gun");
				ShowKillOrKilled();
				
			}else if(KillerPlayerState != MyPlayerState && VictimPlayerState == MyPlayerState)
			{
				KillOrKilledMessage = FString::Printf(TEXT("You were Killed by %s's %s"), *KillerPlayerState->GetShortPlayerName(), "Gun");
				ShowKillOrKilled();
			}
		}
	}


}

void ALaylaHUD::DrawDeathMessages()
{
	UVerticalBox* VBox_DeathMessages = nullptr;
	if (CharacterHUD)
	{
		 VBox_DeathMessages = Cast<UVerticalBox>(
			CharacterHUD->WidgetTree->FindWidget("VerticalBox_DeathMessages"));
	}
	if(VBox_DeathMessages && DeathMessageItemClass)
	{
		VBox_DeathMessages->ClearChildren();
		float CurrTime = GetWorld()->GetTimeSeconds();
		for(int32 i = 0; i < DeathMessages.Num();i++)
		{
			if(DeathMessages[i].HideTime >= CurrTime)
			{
				UUserWidget* MessageItem = CreateWidget<UUserWidget>(GetWorld(), DeathMessageItemClass);
				if (MessageItem)
				{
					UTextBlock* Killer = Cast<UTextBlock>(MessageItem->WidgetTree->FindWidget("TextBlock_Killer"));
					UTextBlock* Weapon = Cast<UTextBlock>(MessageItem->WidgetTree->FindWidget("TextBlock_Weapon"));
					UTextBlock* Victim = Cast<UTextBlock>(MessageItem->WidgetTree->FindWidget("TextBlock_Victim"));
					Killer->SetText(FText::FromString(DeathMessages[i].KillerDesc));
					Weapon->SetText(FText::FromString(DeathMessages[i].DamageType->GetName()));
					Victim->SetText(FText::FromString(DeathMessages[i].VictimDesc));
					VBox_DeathMessages->AddChildToVerticalBox(MessageItem);
				}
			}
		}
	}
}

void ALaylaHUD::DrawKillOrKilled()
{
	if(CharacterHUD)
	{
		UTextBlock* KillOrKilled = Cast<UTextBlock>(CharacterHUD->WidgetTree->FindWidget("TextBlock_KillOrKilled"));
		if(KillOrKilled)
		{
			KillOrKilled->SetText(FText::FromString(KillOrKilledMessage));
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
	APlayerController* OwnerPC = GetOwningPlayerController();
	bGameMenuVisible = !bGameMenuVisible;
	if(GameMenu && OwnerPC){
		if(bGameMenuVisible == true)
		{
			OwnerPC->bShowMouseCursor = true;
			GameMenu->SetVisibility(ESlateVisibility::Visible);
		}else
		{
			OwnerPC->bShowMouseCursor = false;
			GameMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ALaylaHUD::ToggleWinVisibility(bool bVisible)
{
	if(CharacterHUD)
	{
		UBorder* WinBorder = Cast<UBorder>(CharacterHUD->WidgetTree->FindWidget("Border_Win"));
		if(WinBorder){

			if(bVisible)
			{
				WinBorder->SetVisibility(ESlateVisibility::Visible);
			}else
			{
				WinBorder->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void ALaylaHUD::ToggleLoseVisibility(bool bVisible)
{
	if(CharacterHUD)
	{
		UBorder* WinBorder = Cast<UBorder>(CharacterHUD->WidgetTree->FindWidget("Border_Lose"));
		if(WinBorder){

			if(bVisible)
			{
				WinBorder->SetVisibility(ESlateVisibility::Visible);
			}else
			{
				WinBorder->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void ALaylaHUD::ShowKillOrKilled()
{
	bKillOrKilledVisible = true;
	const int KillOrKilledMessageDuration = 3.0f;
	GetWorldTimerManager().SetTimer(TimerHandle_KillOrKilled, this, &ALaylaHUD::HideKillOrKilled, KillOrKilledMessageDuration, false);

	if(CharacterHUD)
	{
		UTextBlock* KillOrKilled = Cast<UTextBlock>(CharacterHUD->WidgetTree->FindWidget("TextBlock_KillOrKilled"));
		if(KillOrKilled)
		{
			KillOrKilled->SetText(FText::FromString(KillOrKilledMessage));
		}

		

		UBorder* Border_KillOrKilled = Cast<UBorder>(CharacterHUD->WidgetTree->FindWidget("Border_KillOrKilled"));
		if(Border_KillOrKilled)
		{
			Border_KillOrKilled->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ALaylaHUD::HideKillOrKilled()
{
	bKillOrKilledVisible = false;

	if(CharacterHUD)
	{

		UBorder* Border_KillOrKilled = Cast<UBorder>(CharacterHUD->WidgetTree->FindWidget("Border_KillOrKilled"));
		if(Border_KillOrKilled)
		{
			Border_KillOrKilled->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

