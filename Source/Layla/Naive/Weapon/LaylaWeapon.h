// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layla/Naive/LaylaEquipment.h"
#include "LaylaWeapon.generated.h"

struct FGameplayTagContainer;

/**
 * 
 */
UCLASS(Abstract)
class LAYLA_API ULaylaWeapon : public ULaylaEquipment
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLaylaEquipmentActorToSpawn>DefaultHandHeldActorsToSpawn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<AActor>> HandHeldActors;
	
	bool bPendingDrawingOrStowing = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_Attacking)
	bool bAttacking = false;

	UPROPERTY(ReplicatedUsing=OnRep_HandHeld)
	bool bHandHeld = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float LastAttackTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CoolingTime = 0.8f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> HandheldAnimSet;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> NonHandHeldAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* CharacterDrawingMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* CharacterStowingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* CharacterAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	UAnimMontage* WeaponAttackMontage;


	/** Functions */
	ULaylaWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	virtual void OnEquipped() override;
	
	virtual void OnUnequipped() override;
	
	virtual void StartAttack();
	
	virtual void StopAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerStartAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerStopAttack();
	
	UFUNCTION()
	void OnRep_HandHeld(bool bPrevHandHeld);

	UFUNCTION()
	void OnRep_Attacking(bool bPrevAttacking);

	// Audio Resources to Play when Holded
	// UAudioxxx...
	virtual float PlayCharacterMontage(UAnimMontage* MontageToPlay) const;
	virtual float PlayWeaponMontage(UAnimMontage* MontageToPlay) const;
	virtual void StopCharacterMontage(UAnimMontage* MontageToStop) const;
	virtual void StopWeaponMontage(UAnimMontage* MontageToStop) const;
	
	UFUNCTION(BlueprintCallable)
	void LinkOwnerPawnToAnimSet() const;


	
private:

	void SetLastAttackTime();
	void SetPendingToFalse();
	void PlayHandHeldAudio();

	// 
	FTimerHandle TimerHandle_Pending;
	FTimerHandle TimerHandle_HandHeldAudio;

	// Attack
	FTimerHandle TimerHandle_SetLastAttackTime;
	FTimerHandle TimerHandle_CharacterAttackMontage;
	FTimerHandle TimerHandle_WeaponAttackMontage;
	FTimerHandle TimerHandle_WeaponAttackAudio;
	FTimerHandle TimerHandle_WeaponAttackFX;
};
