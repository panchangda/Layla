// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaylaPickup.generated.h"

class ULaylaEquipmentManager;
class ULaylaEquipment;
class UWidgetComponent;

UCLASS(BlueprintType, Blueprintable)
class LAYLA_API ALaylaPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaylaPickup();

	//Visual representation of the pickup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Pickup|Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layla|Pickup|Mesh")
	FVector MeshScale;

	// Tips widget for interact  
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Pickup|Widget")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Layla|Pickup|Widget")
	FVector2D DrawSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layla|Pickup|Widget")
	TSubclassOf<UUserWidget> InteractWidget;
	
	UFUNCTION(BlueprintCallable)
	void ToggleInteractWidgetVisibility(bool Visibility);

	UFUNCTION(BlueprintCallable)
	void OnFocus();

	UFUNCTION(BlueprintCallable)
	void OnFocusLost();

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Pickup")
	TSubclassOf<ULaylaEquipment> EquipmentClass;
	
	void OnPick();
	void OnDrop();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
};
