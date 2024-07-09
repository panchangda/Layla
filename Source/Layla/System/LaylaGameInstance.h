// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LaylaGameInstance.generated.h"

// USTRUCT(BlueprintType)
// struct FLaylaCharacterStruct
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
// 	FText DisplayName;
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
// 	FText Description;
// 	
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
// 	UTexture2D* Image;
//
// 	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
// 	TSubclassOf<ACharacter> CharacterBP;
// };

USTRUCT(BlueprintType)
struct FLaylaHeroStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FLaylaHeroStruct() {}


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Image;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ACharacter> CharacterBP;
};

/**
 * 
 */
UCLASS()
class LAYLA_API ULaylaGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Hero")
	FLaylaHeroStruct Hero;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hero")
	UDataTable* HeroDataTable;

	virtual void Init() override;

	void SetDefaultHero();

	FLaylaHeroStruct GetHero();
	
private:

};
