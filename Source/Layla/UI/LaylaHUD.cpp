// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameType/FreeForAll/LaylaGameState_FreeForAll.h"


void ALaylaHUD::DrawHUD()
{
	Super::DrawHUD();

}

void ALaylaHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UIClass)
	{
		UIInstance = CreateWidget<UUserWidget>(GetWorld(), UIClass);
		if (UIInstance)
		{
			UIInstance->AddToViewport();
		}
	}
}
