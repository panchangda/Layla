// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "LaylaCharacter.generated.h"

class ALaylaGun;
class ULaylaEquipment;
class ULaylaEquipmentManager;
class ULaylaWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ULaylaAbilitySystem;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALaylaCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ALaylaCharacter();

	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY(Transient)
	TObjectPtr<ULaylaAbilitySystem> AbilitySystemComponent;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;



	


	// Chracter Infos: BP Helper Functions
	UFUNCTION(BlueprintCallable)
	int32 GetPrimaryAmmoInMagazine();

	UFUNCTION(BlueprintCallable)
	int32 GetSecondaryAmmoInMagazine();

	UFUNCTION(BlueprintCallable)
	ALaylaGun* GetPrimaryGun();

	UFUNCTION(BlueprintCallable)
	ALaylaGun* GetSecondaryGun();
	
	// UFUNCTION(BlueprintCallable)
	// int32 GetPrimaryAmmoInInventory();
	//
	// UFUNCTION(BlueprintCallable)
	// int32 GetSecondaryAmmoInInventory();
	
	UFUNCTION(BlueprintCallable)
	TArray<class ALaylaGun*> GetGunList();
	
	UFUNCTION(BlueprintCallable)
	int32 GetHealth();

	// UFUNCTION(BlueprintCallable)
	// int32 GetEquipment();
	//
	// UFUNCTION(BlueprintCallable)
	// int32 GetInventory();
	
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ADSAction;
	
	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Interact Equip Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipPrimaryAction;

	/** Interact Equip Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryAction;
	
	/** Interact Equip Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipMeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropGunAction;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Layla|Components" , meta=(AllowPrivateAccess = "true"))
	// TObjectPtr<ULaylaEquipmentManager> EquipmentManager;
	
	UFUNCTION(BlueprintCallable)
	bool LineTraceAlongCamera(FHitResult& HitResult);


	

	/** 玩家的最大生命值。这是玩家的最高生命值。此值为玩家出生时的生命值。*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;
 
	/** 玩家的当前生命值。降到0就表示死亡。*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth = 100.0f;
 
	/** RepNotify，用于同步对当前生命值所做的更改。*/ /** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
	UFUNCTION()
	void OnRep_CurrentHealth();

	/** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
	void OnHealthUpdate();
	
	/** 最大生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
 
	/** 当前生命值的取值函数。*/
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
 
	/** 当前生命值的存值函数。将此值的范围限定在0到MaxHealth之间，并调用OnHealthUpdate。仅在服务器上调用。*/
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(float healthValue);
 
	/** 承受伤害的事件。从APawn覆盖。*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage( float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) override;






	/* Guns */
	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<class ALaylaGun*> GunList;
	/* Gun Interactions: Pick, Drop, Equip...... */
	// [Client -> Server]
	void PickGun(ALaylaGun* Gun);
	// [Client -> Server]
	void DropGun(ALaylaGun* Gun);
	// [Server]
	UFUNCTION(Server, Reliable, WithValidation)
	void EquipGun(ALaylaGun* Gun);
	// [Server]
	UFUNCTION(Server, Reliable, WithValidation)
	void UnEquipGun(ALaylaGun* Gun);

	ALaylaGun* FindGun(const FGameplayTag& GunTag);
	
	// [Client + Server]
	void SetCurrentGun(ALaylaGun* NewWeapon, ALaylaGun* LastWeapon);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentGun)
	ALaylaGun* CurrentGun;
	
	UFUNCTION()
	void OnRep_CurrentGun(ALaylaGun* LastWeapon);
	
	// [Server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickGun(ALaylaGun* Gun);
	// [Server]
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropGun(ALaylaGun* Gun);

	// [Server]
	void AddGun(ALaylaGun* Gun);
	// [Server]
	void RemoveGun(ALaylaGun* Gun);


	/* Gun Core: Start/Stop Fire, Reload */
	void StartGunFire();
	void StopGunFire();
	bool bIsGunFiring = false;

	void StartGunReload();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// for animation update
	void StartCrouch();
	void EndCrouch();
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void UpdateCrouchState(const FInputActionValue& Value);
	bool isCrouching;
	
	// interact with interactable objects
	void Interact(const FInputActionValue& Value);
	bool isInteracting;
	
	// Weapon related actions
	void Aim(const FInputActionValue& Value);
	bool isAiming;
	
	void StartFire(const FInputActionValue& Value);
	void StopFire(const FInputActionValue& Value);
	bool isFiring;
	
	void Reload(const FInputActionValue& Value);
	bool isReloading;


	void EquipPrimaryWeapon(const FInputActionValue& Value);
	void EquipSecondaryWeapon(const FInputActionValue& Value);
	void EquipMeleeWeapon(const FInputActionValue& Value);
	void DropGun(const FInputActionValue& Value);
	
	void StartADS(const FInputActionValue& Value);
	void StopADS(const FInputActionValue& Value);
 
	// AnimInstance's GameplayTagPropertyMap only register delegate with ContainerMap(not replicated)
	// Native Function AddOrRemove Replicated GameTag is not working, so we have to make it NetMultiCast RPC manually
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddTag(const FName& TagName);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveTag(const FName& TagName);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiAddTag(const FName& TagName);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiRemoveTag(const FName& TagName);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaSeconds) override;

	AActor* PrevHitActor = nullptr; 
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FVector CameraLocation;
	FRotator CameraRotation;
};

