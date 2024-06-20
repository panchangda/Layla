// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaylaEquipmentManager.generated.h"

class ULaylaEquipment;
class ULaylaWeapon;

UENUM()
enum class EEquipmentInput : uint8
{
	StartAttack,
	StopAttack,
	Reload,
	Drop,
	EquipPrimary,
	EquipSecondary,
	EquipMelee,
	MAX
};
// UENUM()
// enum EHeroEquipmentType : uint8
// {
// 	PrimaryWeapon, SecondaryWeapon, MeeleeWeapon,
// 	Backpack, Armor
// };

// USTRUCT()
// struct FHeroEquipment
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment|Weapon")
// 	ULaylaEquipment* PrimaryWeapon;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment|Weapon")
// 	ULaylaEquipment* SecondaryWeapon;
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment|Weapon")
// 	ULaylaEquipment* MeleeWeapon;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment")
// 	ULaylaEquipment* Backpack;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment")
// 	ULaylaEquipment* Armor;
// };


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAYLA_API ULaylaEquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULaylaEquipmentManager();

	UPROPERTY(Transient, Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<class ULaylaEquipment*> EquipmentList;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULaylaWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentEquipment)
	ULaylaEquipment* CurrentEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int RepVar;

	// Server Only
	UFUNCTION(Server, Reliable, WithValidation)
	void EquipItem(const FString& ItemTypeToEquip);
	
	//Server Only
	UFUNCTION(Server, Reliable, WithValidation)
	void AddItem(TSubclassOf<ULaylaEquipment> ItemClass);

	ULaylaEquipment* FindItem(const FString& ItemTypeToFind);
	
	// UFUNCTION(BlueprintCallable)
	// void UnEquipItem(const FString& ItemTypeToUnEquip);
	
	UFUNCTION(BlueprintCallable)
	ULaylaEquipment* GetCurrentEquipment();

	UFUNCTION(BlueprintCallable)
	ULaylaWeapon* GetCurrentWeapon();
	
	
	UFUNCTION()
	void OnRep_CurrentEquipment(ULaylaEquipment* PrevEquipment);

	// [Local + Server]
	void ChangeCurrentEquipment(ULaylaEquipment* PrevEquipment, ULaylaEquipment* CurrEquipment);
	
	void SpawnDefaultEquipment();

	void HandleInputToEquip(const EEquipmentInput& Input);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
};
