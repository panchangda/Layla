// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaInventoryItem.h"


// Sets default values
ALaylaInventoryItem::ALaylaInventoryItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALaylaInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaylaInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

