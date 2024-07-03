// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaPickup.h"

#include "Components/WidgetComponent.h"


// Sets default values
ALaylaPickup::ALaylaPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the Static Mesh Component.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	// Set the Static Mesh Component as the Root Component.
	RootComponent = StaticMeshComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);

	WidgetComponent->SetVisibility(false);
}

void ALaylaPickup::ToggleInteractWidgetVisibility(bool Visibility)
{
	WidgetComponent->SetVisibility(Visibility);
}

void ALaylaPickup::OnFocus()
{
	ToggleInteractWidgetVisibility(true);
}

void ALaylaPickup::OnFocusLost()
{
	ToggleInteractWidgetVisibility(false);
}

void ALaylaPickup::OnPick()
{
	
}

void ALaylaPickup::OnDrop()
{
	
}

// Called when the game starts or when spawned
void ALaylaPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaylaPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

