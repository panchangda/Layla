// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGameInstance.h"

#include "K2Node_GetDataTableRow.h"

void ULaylaGameInstance::Init()
{
	Super::Init();

	SetDefaultHero();
}

void ULaylaGameInstance::SetDefaultHero()
{
	
	if (HeroDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HeroDataTable is null! Please assign the DataTable in the editor."));
		return;
	}

	// Use First Row as Default Hero
	// 获取所有行的名称
	TArray<FName> RowNames = HeroDataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HeroDataTable is empty!"));
		return;
	}

	// 获取第一行的数据
	FLaylaHeroStruct* Row = HeroDataTable->FindRow<FLaylaHeroStruct>(RowNames[0], TEXT(""));

	if (Row)
	{
		Hero = *Row;
		UE_LOG(LogTemp, Log, TEXT("Default Hero set: %s"), *Hero.DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find the first row in the HeroDataTable"));
	}
}

FLaylaHeroStruct ULaylaGameInstance::GetHero()
{
	return Hero;
}
