// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "LaylaGun.generated.h"


class ALaylaCharacter;
class UWidgetComponent;

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

USTRUCT()
struct FGunConfig
{
	GENERATED_BODY()

	UPROPERTY()
	float MagazineSize;

	UPROPERTY()
	float FireRate;

	UPROPERTY()
	bool ReserveAmmo;
};


UCLASS()
class LAYLA_API ALaylaGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaylaGun();

	// SKM represetations
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	// Interactions  
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DrawSize;

	void OnFocused();
	void OnFocuseLost();

	// Tag
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GunTag;

	UPROPERTY(EditDefaultsOnly)
	FVector MeshScale = FVector(1.f, 1.f ,1.f);
	
	// [Server + Client OnRep_OwnerPawn call]
	void OnDrop(APawn* PrevOwner);
	// [Server + Client OnRep_OwnerPawn call]
	void OnPick(APawn* NewOwner);
	bool bPendingEquip = false;
	bool bIsEquipped = false;
	// [Server + Client OnRep_CurrentWeapon call]
	void OnEquip();
	// [Server + Client OnRep_CurrentWeapon call]
	void OnUnEquip();
	
	
	UPROPERTY(ReplicatedUsing=OnRep_OwnerPawn)
	APawn* OwnerPawn;
	UFUNCTION()
	void OnRep_OwnerPawn(APawn* PrevPawn);

	void SetOwnerPawn(APawn* NewOwner);
	APawn* GetOwnerPawn();

	/**
	 * Fire & Reload & Ammos
	 * 
	 */
	
	/** current ammo - inside clip */
	UPROPERTY(Transient, Replicated)
	int32 AmmoInMagzine;
	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCounter)
	int32 BurstCounter;
	UFUNCTION()
	void OnRep_BurstCounter();
	
	/** determine current weapon state */
	void DetermineWeaponState();
	void SetWeaponState(EWeaponState::Type NewState	);

	void OnBurstStarted();
	void OnBurstFinished();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();
	
	virtual void Fire();
	
	void HandleFiring();

	float LastFireTime;
	/** wants to fire? */
	bool bWantsToFire = false;
	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
	bool bPendingReload = true;
	UFUNCTION()
	void OnRep_Reload();

	bool CanFire();
	bool CanReload();
	
	virtual void StartFire();
	virtual void StopFire();
	virtual void StartReload(bool bFromReplication = false);
	virtual void StopReload();
	// [Server Only]
	void ReloadGun();
	
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();
	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();
	UFUNCTION(reliable, server, WithValidation)
	void ServerStopReload();
	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFiring();

	FGunConfig GunConfig;

	
	
	void AttachToPawn(FName Socket, FTransform Transform);
	void DetachFromPawn();
	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket_Body;
	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket_Hand;
	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform_Body;
	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform_Hand;


	/** animation linked layers */
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> ArmedAnimSet;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> UnArmedAnimSet;
	
	/** animation played on pawn & gun */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* CharacterEquipAnim;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* CharacterUnEquipAnim;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* CharacterFireAnim;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* CharacterReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* GunFireAnim;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	UAnimMontage* GunReloadAnim;
	float PlayCharacterAnim(UAnimMontage* AnimToPlay);
	float PlayWeaponAnim(UAnimMontage* AnimToPlay);
	void StopCharacterAnim(UAnimMontage* AnimToStop);
	void StopWeaponAnim(UAnimMontage* AnimToStop);
	void LinkOwnerPawnToAnimSet(TSubclassOf<UAnimInstance> AnimSetToLink) const;
	void LinkPawnToAnimSet(TSubclassOf<UAnimInstance> AnimSetToLink, APawn* Pawn) const;



	/***
	 * Audio
	 */
	
	/** play weapon sounds */
	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* OutOfAmmoSound;

	/** reload sound */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ReloadSound;
	
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;
	FTimerHandle TimerHandle_HandleFiring;
	FTimerHandle TimerHandle_StopReload;
	FTimerHandle TimerHandle_ReloadWeapon;
	void OnEquipFinished();
};
