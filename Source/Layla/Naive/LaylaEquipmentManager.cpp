// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaEquipmentManager.h"
#include "LaylaEquipment.h"
#include "LaylaWeapon.h"


// Sets default values for this component's properties
ULaylaEquipmentManager::ULaylaEquipmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	EquipmentMap.Empty();
}


void ULaylaEquipmentManager::ChangeCurrentWeapon(FString EquipmentTag)
{
	if(ULaylaEquipment** Item = EquipmentMap.Find(EquipmentTag))
	{
		if((*Item)->IsA(ULaylaWeapon::StaticClass()))
		{
			if(CurrentWeapon)
			{
				CurrentWeapon->OnUnequipped();
			}
			CurrentWeapon = Cast<ULaylaWeapon>(*Item);
			CurrentWeapon->OnEquipped();
		}
	}
}

void ULaylaEquipmentManager::EquipmentItem(TSubclassOf<ULaylaEquipment> ItemClass)
{
	ULaylaEquipment* ItemInstance = NewObject<ULaylaEquipment>(this->GetOwner(), ItemClass);
	FString ItemClassTag = ItemInstance->EquipmentTypeString;
	if(ULaylaEquipment** ItemPtr = EquipmentMap.Find(ItemClassTag))
	{
		ULaylaEquipment* Item = *ItemPtr;
		// Is Same Equipment? Shotgun & Rifle have same ItemClassTag,
		if(ItemClass == Item->GetClass())
		{
			if(Item->IsA(ULaylaWeapon::StaticClass()))
			{
				// CurrentWeapon->FillAmmos?
			}else
			{
				
			}

		}else
		{
			Item->OnUnequipped();
			
			Item = ItemInstance;
			ItemInstance->OnEquipped();
		}
		
			// if(CurrentWeapon)
			// {
			// 	CurrentWeapon->OnUnequipped();
			// }
			// CurrentWeapon = Cast<ULaylaWeapon>(*Item);
			// CurrentWeapon->OnEquipped();
	}else
	{
		EquipmentMap.Add(ItemClassTag, ItemInstance);
		if(ItemInstance->IsA(ULaylaWeapon::StaticClass()))
		{
			CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
			CurrentWeapon->OnEquipped();
		}
	}
}

void ULaylaEquipmentManager::UnEquipmentItem(FString EquipmentTag)
{
	if(ULaylaEquipment** Item = EquipmentMap.Find(EquipmentTag))
	{
		(*Item)->OnUnequipped();
		if((*Item)->IsA(ULaylaWeapon::StaticClass()))
		{
			CurrentWeapon = Cast<ULaylaWeapon>(*Item);
		}
	}
}

void ULaylaEquipmentManager::CurrentWeaponFire()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}

void ULaylaEquipmentManager::CurrentWeaponReload()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

// Called when the game starts
void ULaylaEquipmentManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULaylaEquipmentManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

