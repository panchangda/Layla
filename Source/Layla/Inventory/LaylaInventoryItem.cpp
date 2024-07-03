// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaInventoryItem.h"

#include "Components/WidgetComponent.h"


// Sets default values
ALaylaInventoryItem::ALaylaInventoryItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the Static Mesh Component.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	// Set the Static Mesh Component as the Root Component.
	RootComponent = StaticMeshComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetVisibility(false);

	if(WidgetClass)
	{
		WidgetComponent->SetWidgetClass(WidgetClass);
	}
}

void ALaylaInventoryItem::ToggleInteractWidgetVisibility(bool Visibility)
{
}

void ALaylaInventoryItem::OnFocused()
{
	ToggleInteractWidgetVisibility(true);
}

void ALaylaInventoryItem::OnFocusLost()
{
	ToggleInteractWidgetVisibility(false);
}

void ALaylaInventoryItem::OnPick()
{
	
}

void ALaylaInventoryItem::OnDrop()
{
	
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

