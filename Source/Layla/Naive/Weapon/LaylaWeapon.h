// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layla/Naive/LaylaEquipment.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	
	
	virtual void OnEquipped() override;
	
	virtual void OnUnequipped() override;
	
	
	void StartFire(FVector* Location, FRotator* Rotation);

	UFUNCTION(BlueprintCallable)
	void StopFire();

	void PlayFireMontage();
	
	UFUNCTION(BlueprintCallable)
	void Reload();
	
	bool bPendingEquip = false;

	UPROPERTY(ReplicatedUsing=OnRep_Holding)
	bool bHolding = true;

	UFUNCTION()
	void OnRep_Holding(bool bHoldingState);

	UFUNCTION(BlueprintCallable)
	void LinkOwnerPawnToAnimSet(bool bHolding) const;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> ArmedAnimSet;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> UnarmedAnimSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Weapon")
	int MaxAmmo=1;
	UPROPERTY(Replicated, EditAnywhere, Category="Layla|Weapon")
	int CurrAmmoInBackpack=1;
	UPROPERTY(Replicated, EditAnywhere, Category="Layla|Weapon")
	int CurrAmmoInMagazine=1;
	UPROPERTY(Replicated, EditAnywhere, Category="Layla|Weapon")
	int MagazineSize=0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Weapon")
	float FireRate = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Weapon")
	bool bIsRangedWeapon = true;

	
	UPROPERTY(ReplicatedUsing=OnRep_BIsReloading)
	bool bIsReloading = false;
	UPROPERTY(ReplicatedUsing=OnRep_BIsFiring)
	bool bIsFiring = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Layla|Weapon")
	TSubclassOf<class ALaylaProjectile> ProjectileClass;

	FVector* CameraLocation;
	FRotator* CameraRotation;
	
	float LastFireTime;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FLaylaAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	FLaylaAnimLayerSelectionSet UneuippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* WeaponEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* WeaponUnEquipMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* WeaponReloadMontage;
	
	// Choose the best layer from EquippedAnimSet or UneuippedAnimSet based on the specified gameplay tags
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<FLaylaEquipmentActorToSpawn> HandHeldActorToSpawn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<AActor>> HandHeldActors;

	
private:
	FTimerHandle PlayMontageTimer;
	FTimerHandle GenerateProjectileTimer;
	FTimerHandle WeaponReloadTimer;
	FTimerHandle WeaponReloadResetTimer;

	
	UFUNCTION()
	void OnRep_BIsReloading();

	UFUNCTION()
	void OnRep_BIsFiring();
	
	void ResetReloadState();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerWeaponReload();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGenerateProjectile();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();


	

};
