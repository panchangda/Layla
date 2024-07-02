// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LaylaCharacter.h"
#include "LaylaPlayerState.h"
#include "UI/LaylaHUD.h"

class UEnhancedInputLocalPlayerSubsystem;

void ALaylaPlayerController::OnDeathMessage(ALaylaPlayerState* KillerPlayerState,
	ALaylaPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	ALaylaHUD* LaylaHUD = GetLaylaHUD();
	if (LaylaHUD)
	{
		LaylaHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);		
	}
}

void ALaylaPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void ALaylaPlayerController::Suicide()
{
	if ( IsInState(NAME_Playing) )
	{
		ServerSuicide();
	}
}

void ALaylaPlayerController::ServerSuicide_Implementation()
{
	if ( (GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)) )
	{
		ALaylaCharacter* MyPawn = Cast<ALaylaCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

bool ALaylaPlayerController::ServerSuicide_Validate()
{
	return true;
}

void ALaylaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{

		// ScoreBoard
		EnhancedInputComponent->BindAction(ScoreBoardAction, ETriggerEvent::Started, this, &ALaylaPlayerController::ShowScoreBoard);
		EnhancedInputComponent->BindAction(ScoreBoardAction, ETriggerEvent::Completed, this, &ALaylaPlayerController::HideScoreBoard);

		// GameMenu
		EnhancedInputComponent->BindAction(GameMenuAction, ETriggerEvent::Triggered, this, &ALaylaPlayerController::ToggleGameMenuVisibility);
	}
	
}

ALaylaHUD* ALaylaPlayerController::GetLaylaHUD()
{
	return Cast<ALaylaHUD>(GetHUD());
}

void ALaylaPlayerController::ShowScoreBoard()
{
	ALaylaHUD* LaylaHUD = GetLaylaHUD();
	if(LaylaHUD)
	{
		LaylaHUD->ShowScoreBoard();
	}
}

void ALaylaPlayerController::HideScoreBoard()
{
	ALaylaHUD* LaylaHUD = GetLaylaHUD();
	if(LaylaHUD)
	{
		LaylaHUD->HideScoreBoard();
	}
}

void ALaylaPlayerController::ToggleGameMenuVisibility()
{
	ALaylaHUD* LaylaHUD = GetLaylaHUD();
	if(LaylaHUD)
	{
		bShowMouseCursor = true;
		LaylaHUD->ToggleGameMenuVisibility();
	}
}

