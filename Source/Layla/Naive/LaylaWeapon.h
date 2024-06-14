// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaylaEquipment.h"
#include "LaylaWeapon.generated.h"

struct FGameplayTagContainer;

USTRUCT(BlueprintType)
struct FLaylaAnimLayerSelectionSet
{
	GENERATED_BODY()
	

	// The layer to use if none of the LayerRules matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;
};

/**
 * 
 */
UCLASS()
class LAYLA_API ULaylaWeapon : public ULaylaEquipment
{
	GENERATED_BODY()
	
public:
	ULaylaWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void OnEquipped() override;
	
	virtual void OnUnequipped() override;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void Reload();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FLaylaAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FLaylaAnimLayerSelectionSet UneuippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* ReloadMontage;
	
	// Choose the best layer from EquippedAnimSet or UneuippedAnimSet based on the specified gameplay tags
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;
	

	UFUNCTION(Server, Reliable)
	void GenerateBullet();
	
};
