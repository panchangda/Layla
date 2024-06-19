// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LaylaWeapon.h"
#include "LaylaWeapon_Ranged.generated.h"

class ALaylaProjectile;

UENUM()
enum class FRangedWeaponType : uint8
{
	Instance,
	Projectile,
};

USTRUCT(BlueprintType, Blueprintable)
struct FRangedWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxAmmo=0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ClipSize=0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireRate = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDamageDistance = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UDamageType> DamageType = UDamageType::StaticClass();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRangedWeaponType Type = FRangedWeaponType::Instance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ALaylaProjectile> ProjectileType;

	
	FRangedWeaponData():
		MaxAmmo(0),
		ClipSize(0),
		FireRate(1.0f){}
};
/**
 * 
 */
UCLASS()
class LAYLA_API ULaylaWeapon_Ranged : public ULaylaWeapon
{
	GENERATED_BODY()
public:

	// UPROPERTY(Replicated, EditAnywhere)
	// int CurrAmmoInClip=0;
	// UPROPERTY(Replicated, EditAnywhere)
	// int CurrAmmoInBag=0;
	//
	// UPROPERTY(ReplicatedUsing=OnRep_Reloading)
	// bool bIsReloading = false;
	// UPROPERTY(ReplicatedUsing=OnRep_Firing)
	// bool bIsFiring = false;
	//
	//
	//
	// FVector* CameraLocation;
	// FRotator* CameraRotation;
	//
	// float LastFireTime;
	//
	// UFUNCTION()
	// void OnRep_Reloading();
	//
	// UFUNCTION()
	// void OnRep_Firing();
	//
	// UFUNCTION()
	// void OnRep_BIsReloading();
	//
	// UFUNCTION()
	// void OnRep_BIsFiring();
	//
	// void ResetReloadState();
	//
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerWeaponReload();
	//
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerStopFire();
	//
	//
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerGenerateProjectile();
	//
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerFire();
	//
	// // Animation Resources
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	// UAnimMontage* CharacterFireMontage;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	// UAnimMontage* CharacterReloadMontage;
	//
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	// UAnimMontage* WeaponFireMontage;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	// UAnimMontage* WeaponReloadMontage;


};
