// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "LaylaEquipment.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FLaylaEquipmentActorToSpawn
{
	GENERATED_BODY()

	FLaylaEquipmentActorToSpawn(){}

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
	virtual ~ULaylaEquipment() override;
	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(Server, Reliable)
	virtual void OnEquipped();

	UFUNCTION(Server, Reliable)
	virtual void OnUnequipped();

	
	virtual void SpawnEquipmentActors(const TArray<FLaylaEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLaylaEquipmentActorToSpawn> DefaultActorsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString EquipmentTypeString;

		
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
};
