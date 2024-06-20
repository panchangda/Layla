// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LaylaEquipment.generated.h"


class ULaylaEquipmentManager;

USTRUCT(BlueprintType, Blueprintable)
struct FLaylaEquipmentActorToSpawn
{
	GENERATED_BODY()

	FLaylaEquipmentActorToSpawn(){}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket_Body;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket_Hand;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform_Body;
	
	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform_Hand;
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LAYLA_API ULaylaEquipment : public UObject
{
	GENERATED_BODY()

public:
	ULaylaEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~ULaylaEquipment() override;
	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual bool IsSupportedForNetworking() const override;

	ULaylaEquipmentManager* GetManager() const;
	
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	void InitializeEquipment();
	
	virtual void OnEquipped();
	
	virtual void OnUnequipped();

	void AttachToHand();
	void AttachToBody();
	void SpawnEquipmentActors(const TArray<FLaylaEquipmentActorToSpawn>& ActorsToSpawn);
	void DestroyEquipmentActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLaylaEquipmentActorToSpawn> DefaultActorsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString EquipmentTypeString;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
