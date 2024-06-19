// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaEquipmentManager.h"
#include "LaylaEquipment.h"
#include "LaylaWeapon.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
ULaylaEquipmentManager::ULaylaEquipmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Empty Equipment Map
	EquipmentMap.Empty();

	// Default Weapon Class
	DefaultWeaponClass = ULaylaWeapon::StaticClass();
	
	SetIsReplicatedByDefault(true);
}


void ULaylaEquipmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULaylaEquipmentManager, CurrentWeapon)
	DOREPLIFETIME(ULaylaEquipmentManager, RepVar)
}

void ULaylaEquipmentManager::ChangeCurrentWeapon_Implementation(const FString& EquipmentTag)
{
	// Only Execute on Server
	if(! GetOwner()->HasAuthority()) return;
	
	if(ULaylaEquipment** ItemPtr = EquipmentMap.Find(EquipmentTag))
	{
		ULaylaWeapon* WeaponToEquip = Cast<ULaylaWeapon>(*ItemPtr);
		if(WeaponToEquip && WeaponToEquip!= CurrentWeapon)
		{
			if(CurrentWeapon)
			{
				CurrentWeapon->OnUnequipped();
			}
			CurrentWeapon = WeaponToEquip;
			CurrentWeapon->OnEquipped();
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not Change Weapon: Does not Equip Corresponding Weapon Type!"));
	}
}

void ULaylaEquipmentManager::EquipmentItem_Implementation(TSubclassOf<ULaylaEquipment> ItemClass)
{
	// Only Execute on Server
	if(! GetOwner()->HasAuthority()) return;
	
	ULaylaEquipment* ItemInstance = NewObject<ULaylaEquipment>(this, ItemClass);
	FString ItemClassTag = ItemInstance->EquipmentTypeString;

	// Must Manually Add Sub UObject to ReplicateList  
		AddReplicatedSubObject(ItemInstance);
    	// GetOwner()->AddActorComponentReplicatedSubObject(this, ItemInstance, COND_None);
	
	
	// Existing Same Tag Item
	if(ULaylaEquipment** ExistingItemPtr = EquipmentMap.Find(ItemClassTag))
	{
		ULaylaEquipment* ExistingItem = *ExistingItemPtr;
		// Is Same Equipment Class? Shotgun & Rifle have same ItemClassTag, but is different equipment(weapon)
		if(ItemClass == ExistingItem->GetClass())
		{
			if(ExistingItem->IsA(ULaylaWeapon::StaticClass())) 			// Same Weapon Class Process Logic: Fill in Ammos?  
			{
				// CurrentWeapon->FillAmmos?
			}	
			else 			// Same Other Equipment Class Process Logic...
			{
				
			}
		}
		// Existing Same Tag but Different Class:
		else
		{
			ExistingItem->OnUnequipped();

			if(ExistingItem == CurrentWeapon)
			{
				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
			}
			// Replace ExistingItem with new ItenInstance
			EquipmentMap[ItemClassTag] = ItemInstance;
			ItemInstance->OnEquipped();
		}
	}
	// No Existing Same Tag Equipment:
	else
	{
		EquipmentMap.Add(ItemClassTag, ItemInstance);
		if(ItemInstance->IsA(ULaylaWeapon::StaticClass())) // if Weapon Class: deal with CurrentWeapon Switch Logic
		{
			
			if(ItemClassTag == "PrimaryWeapon")  // Primary Weapon: Always be CurrentWeapon
			{
				if(CurrentWeapon){CurrentWeapon->OnUnequipped();}
				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
				CurrentWeapon->OnEquipped();	
			}else if(ItemClassTag == "SecondaryWeapon" &&  // Secondary Weapon: Only Be CurrentWeapon when No Currrent Weapon or Current Weapon is Melee
				( !CurrentWeapon  ||  CurrentWeapon->EquipmentTypeString == "MeleeWeapon"))
			{
				if(CurrentWeapon){CurrentWeapon->OnUnequipped();}
				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
				CurrentWeapon->OnEquipped();	
			}else if(ItemClassTag == "MeleeWeapon" &&  !CurrentWeapon) // Melee Weapon: Only be CurrentWeapon when No Current Weapon
			{
				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
				CurrentWeapon->OnEquipped();	
			}
		}else
		{
			// Equip new ItemInstance
			ItemInstance->OnEquipped();
		}
	}
}

void ULaylaEquipmentManager::UnEquipmentItem(FString EquipmentTag)
{
	if(ULaylaEquipment** ItemPtr = EquipmentMap.Find(EquipmentTag))
	{
		ULaylaEquipment* Item = *ItemPtr;
		Item->OnUnequipped();
		if(Item->IsA(ULaylaWeapon::StaticClass()))
		{
			CurrentWeapon = Cast<ULaylaWeapon>(Item);
		}
	}
}

ULaylaWeapon* ULaylaEquipmentManager::GetCurrentWeapon()
{
	return CurrentWeapon;
}


void ULaylaEquipmentManager::OnRep_CurrentWeapon(ULaylaWeapon* PrevWeapon)
{
	if(PrevWeapon)
	{
		PrevWeapon->OnUnequipped();
	}
	if(CurrentWeapon)
	{
		CurrentWeapon->OnEquipped();
	}
}

// Called when the game starts
void ULaylaEquipmentManager::BeginPlay()
{
	Super::BeginPlay();
	
	EquipmentItem(DefaultWeaponClass);
	
	// CurrentWeapon = NewObject<ULaylaWeapon>(this->GetOwner(), DefaultWeaponClass);
	// EquipmentMap.Add(CurrentWeapon->EquipmentTypeString, CurrentWeapon);
	// CurrentWeapon->OnEquipped();
}


// Called every frame
void ULaylaEquipmentManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



