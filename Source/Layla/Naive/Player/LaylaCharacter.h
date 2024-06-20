// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "LaylaCharacter.generated.h"

class ULaylaEquipment;
class ULaylaEquipmentManager;
class ULaylaWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALaylaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Layla|Components" , meta=(AllowPrivateAccess = "true"))
	TObjectPtr<ULaylaEquipmentManager> EquipmentManager;
	
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


public:
	ALaylaCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// for animation update
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
	
	void EquipWeapon(const FString& WeaponType);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FVector CameraLocation;
	FRotator CameraRotation;
};

