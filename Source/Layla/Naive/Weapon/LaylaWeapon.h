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
	
	bool bPendingEquip = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_Attacking)
	bool bAttacking = false;

	UPROPERTY(ReplicatedUsing=OnRep_Equip)
	bool bEquip = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float LastAttackTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CoolingTime = 0.8f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> ArmedAnimSet;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UAnimInstance> UnArmedAnimSet;

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
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnEquipped() override;
	
	virtual void OnUnequipped() override;
	
	virtual void StartAttack();
	
	virtual void StopAttack();
	
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerStartAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerStopAttack();

	void DetermineWeaponState();

	UFUNCTION()
	void OnRep_Equip();
	
	UFUNCTION()
	void OnRep_Attacking(bool bPrevAttacking);

	// Audio Resources to Play when Holded
	// UAudioxxx...
	virtual float PlayCharacterMontage(UAnimMontage* MontageToPlay) const;
	virtual float PlayWeaponMontage(UAnimMontage* MontageToPlay) const;
	virtual void StopCharacterMontage(UAnimMontage* MontageToStop) const;
	virtual void StopWeaponMontage(UAnimMontage* MontageToStop) const;
	
	UFUNCTION(BlueprintCallable)
	void LinkOwnerPawnToAnimSet(TSubclassOf<UAnimInstance> AnimSetToLink) const;
	
private:

	void SetLastAttackTime();
	void SetPendingToFalse();
	void PlayEquipFinishAudio();

	// 
	FTimerHandle TimerHandle_Pending;
	FTimerHandle TimerHandle_EquipFinishAudio;

	// Attack
	FTimerHandle TimerHandle_SetLastAttackTime;
	FTimerHandle TimerHandle_CharacterAttackMontage;
	FTimerHandle TimerHandle_WeaponAttackMontage;
	FTimerHandle TimerHandle_WeaponAttackAudio;
	FTimerHandle TimerHandle_WeaponAttackFX;
};
