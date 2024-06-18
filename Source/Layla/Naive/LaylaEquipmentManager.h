// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaylaEquipmentManager.generated.h"

class ULaylaEquipment;
class ULaylaWeapon;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Layla|Equipment")
	// TArray<ULaylaEquipment* > EquipmentList;
	TMap<FString, ULaylaEquipment*> EquipmentMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layla|Equipment")
	TSubclassOf<ULaylaWeapon> DefaultWeaponClass;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ChangeCurrentWeapon(const FString& EquipmentTag);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EquipmentItem(TSubclassOf<ULaylaEquipment> ItemClass);
	
	UFUNCTION(BlueprintCallable)
	void UnEquipmentItem(FString EquipmentTag);
	
	UFUNCTION(BlueprintCallable)
	ULaylaWeapon* GetCurrentWeapon();

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void AddWeapon();
	
	TObjectPtr<ULaylaWeapon> CurrentWeapon;
};
