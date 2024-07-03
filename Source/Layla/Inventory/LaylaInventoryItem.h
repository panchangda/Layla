// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPickupable.h"
#include "GameFramework/Actor.h"
#include "LaylaInventoryItem.generated.h"

UENUM()
enum class FInventoryItemType : uint8
{
	Medicine,
	Ammo,
	// Grenade,
};

USTRUCT()
struct FInventoryItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName Name;
	
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	FInventoryItemType Type;

	UPROPERTY(EditDefaultsOnly)
	FString Description;

	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
};

UCLASS(BlueprintType,Blueprintable)
class LAYLA_API ALaylaInventoryItem : public AActor, public IPickupable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaylaInventoryItem();

	//Visual representation of the pickup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Pickup|Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layla|Pickup|Mesh")
	FVector MeshScaleOnDrop;

	// Tips widget for interact  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Inventory")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layla|Inventory")
	FVector2D DrawSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Inventory")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Layla|Inventory")
	UTexture* ItemIcon;
	
	UFUNCTION(BlueprintCallable)
	void ToggleInteractWidgetVisibility(bool Visibility);


	UPROPERTY(EditDefaultsOnly, Category="Layla|Inventory")
	FInventoryItemType ItemType;

	
	// Pickup Interface Implementation
	virtual void OnFocused() override;
	virtual void OnFocusLost() override;
	
	virtual void OnPick() override;
	virtual void OnDrop() override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
