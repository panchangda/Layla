// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

struct FGameplayTag;

UCLASS()
class LAYLA_API AEquipment : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquipment();
	virtual void OnEquipped();
	virtual void OnUnEquipped();
	FGameplayTag Tag;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
