// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LaylaEquipment.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FLaylaEquipmentActorToSpawn
{
	GENERATED_BODY()

	FLaylaEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
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
	
	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
	
	virtual void OnEquipped();
	virtual void OnUnequipped();
	
	virtual void SpawnEquipmentActors(const TArray<FLaylaEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLaylaEquipmentActorToSpawn DefaultActorsToSpawn;
	
private:
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
