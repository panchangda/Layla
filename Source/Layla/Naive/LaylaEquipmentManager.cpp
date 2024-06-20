// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaEquipmentManager.h"
#include "LaylaEquipment.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/LaylaWeapon.h"
#include "Weapon/LaylaWeapon_Ranged.h"


// Sets default values for this component's properties
ULaylaEquipmentManager::ULaylaEquipmentManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	EquipmentList.Empty();

	// Default Weapon Class
	DefaultWeaponClass = ULaylaWeapon::StaticClass();
	
	SetIsReplicatedByDefault(true);
}


void ULaylaEquipmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULaylaEquipmentManager, EquipmentList)
	DOREPLIFETIME(ULaylaEquipmentManager, CurrentEquipment)
	DOREPLIFETIME(ULaylaEquipmentManager, RepVar)
}

bool ULaylaEquipmentManager::EquipItem_Validate(const FString& ItemTypeToEquip)
{
	return true;
}

void ULaylaEquipmentManager::EquipItem_Implementation(const FString& ItemTypeToEquip)
{
	if(ULaylaEquipment* ItemToEquip = FindItem(ItemTypeToEquip))
	{
		ULaylaEquipment* PrevEquipment = CurrentEquipment;
		if(PrevEquipment == nullptr ||
			PrevEquipment->EquipmentTypeString!= ItemTypeToEquip)
		{
			CurrentEquipment = ItemToEquip;
			ChangeCurrentEquipment(PrevEquipment, CurrentEquipment);
		}
	}
}


void ULaylaEquipmentManager::AddItem_Implementation(TSubclassOf<ULaylaEquipment> ItemClass)
{
	if(!ItemClass) return ;
	ULaylaEquipment* ItemInstance = NewObject<ULaylaEquipment>(this, ItemClass);

	ItemInstance->InitializeEquipment();
	
	FString EquipmentType = ItemInstance->EquipmentTypeString;
	// Must Manually Add Sub UObject to ReplicateList  
	AddReplicatedSubObject(ItemInstance);
	// GetOwner()->AddActorComponentReplicatedSubObject(this, ItemInstance, COND_None);
	
	if(ULaylaEquipment* ExistingSameTypeEquipment = FindItem(EquipmentType))
	{
		RemoveReplicatedSubObject(ItemInstance);
		
		// if(ItemInstance->IsA(ULaylaWeapon::StaticClass())) // Weapon Process Logic
		// {
		// 	if(ItemClass == ExistingSameTypeEquipment->GetClass()) // Same Class: Fill Ammo
		// 	{
		// 		if(ItemInstance->IsA(ULaylaWeapon_Ranged::StaticClass()))
		// 		{
		// 			// Cast<ULaylaWeapon_Ranged>(ItemInstance)->FillAmmos();
		// 		}
		// 		
		// 	}else // Same Type: Replace
		// 	{
		// 		// if(ExistingSameTypeEquipment == CurrentEquipment)
		// 		// {
		// 		// 	ChangeCurrentEquipment(ExistingSameTypeEquipment, ItemInstance);
		// 		// }
		// 		//
		// 		// EquipmentList.Remove(ExistingSameTypeEquipment);
		// 		// EquipmentList.AddUnique(ItemInstance);
		// 	}
		// }else if(0)
		// {
		// 	
		// }
		
	}else
	{
		EquipmentList.AddUnique(ItemInstance);
		
		if(EquipmentType == "PrimaryWeapon")
		{
			EquipItem(EquipmentType);
		}
		else if(EquipmentType == "SecondaryWeapon")
		{
			EquipItem(EquipmentType);
			
		}else if(EquipmentType == "MeleeWeapon")
		{
			EquipItem(EquipmentType);
		}else
		{
			
		}
	}
	

	
}

bool ULaylaEquipmentManager::AddItem_Validate(TSubclassOf<ULaylaEquipment> ItemClass)
{
	return true;
}

ULaylaEquipment* ULaylaEquipmentManager::FindItem(const FString& EquipmentTypeToFind)
{
	for(ULaylaEquipment* item:EquipmentList)
	{
		if(item->EquipmentTypeString == EquipmentTypeToFind)
		{
			return item;
		}
	}
	return nullptr;
}

// void ULaylaEquipmentManager::UnEquipItem(const FString& ItemTypeToUnEquip)
// {
// 	if(ULaylaEquipment* Item = FindItem(ItemTypeToUnEquip))
// 	{
// 		Item->OnUnequipped();
// 	}
// }


ULaylaEquipment* ULaylaEquipmentManager::GetCurrentEquipment()
{
	return CurrentEquipment;
}

ULaylaWeapon* ULaylaEquipmentManager::GetCurrentWeapon()
{
	return Cast<ULaylaWeapon>(GetCurrentEquipment());
}

void ULaylaEquipmentManager::OnRep_CurrentEquipment(ULaylaEquipment* PrevEquipment)
{
	ChangeCurrentEquipment(PrevEquipment, CurrentEquipment);
}

void ULaylaEquipmentManager::ChangeCurrentEquipment(ULaylaEquipment* PrevEquipment, ULaylaEquipment* CurrEquipment)
{
	if(PrevEquipment != CurrentEquipment)
	{
		if(PrevEquipment != nullptr)
		{
			PrevEquipment->OnUnequipped();
		}
			
		CurrentEquipment->OnEquipped();

	}
}


void ULaylaEquipmentManager::SpawnDefaultEquipment()
{
	// EquipmentItem(DefaultWeaponClass);
	AddItem(DefaultWeaponClass);
}

void ULaylaEquipmentManager::HandleInputToEquip(const EEquipmentInput& Input)
{
	if(Input == EEquipmentInput::StartAttack)
	{
		if(ULaylaWeapon* Weapon =  GetCurrentWeapon())
		{
			Weapon->StartAttack();
		}
		
	}else if(Input == EEquipmentInput::StopAttack)
	{
		if(ULaylaWeapon* Weapon =  GetCurrentWeapon())
		{
			Weapon->StopAttack();
		}
	
	}else if(Input == EEquipmentInput::Reload)
	{
		if(ULaylaWeapon* Weapon = GetCurrentWeapon())
		{
			if(Weapon->IsA(ULaylaWeapon_Ranged::StaticClass()))
			{
				// Weapon->StartReload();
			}
		}
		
	}else if(Input == EEquipmentInput::Drop)
	{
		
	}else if(Input == EEquipmentInput::EquipPrimary)
	{
		if(ULaylaWeapon* Weapon = GetCurrentWeapon())
		{
			if(Weapon->EquipmentTypeString != "PrimaryWeapon")
			{
				EquipItem("PrimaryWeapon");
			}
		}
		
	}else if(Input == EEquipmentInput::EquipSecondary)
	{
		if(ULaylaWeapon* Weapon = GetCurrentWeapon())
		{
			if(Weapon->EquipmentTypeString != "SecondaryWeapon")
			{
				EquipItem("SecondaryWeapon");
			}
		}
		
	}else if(Input == EEquipmentInput::EquipMelee)
	{
		if(ULaylaWeapon* Weapon = GetCurrentWeapon())
		{
			if(Weapon->EquipmentTypeString != "MeleeWeapon")
			{
				EquipItem("MeleeWeapon");
			}
		}
		
	}
}

// Called when the game starts
void ULaylaEquipmentManager::BeginPlay()
{
	Super::BeginPlay();
	// CurrentWeapon = NewObject<ULaylaWeapon>(this->GetOwner(), DefaultWeaponClass);
	// EquipmentMap.Add(CurrentWeapon->EquipmentTypeString, CurrentWeapon);
	// CurrentWeapon->OnEquipped();
	SpawnDefaultEquipment();
}


// Called every frame
void ULaylaEquipmentManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}





// void ULaylaEquipmentManager::EquipmentItem_Implementation(TSubclassOf<ULaylaEquipment> ItemClass)
// {
// 	// Only Execute on Server
// 	if(! GetOwner()->HasAuthority()) return;
// 	
// 	
// 	
// 	
// 	// Existing Same Tag Item
// 	if(ULaylaEquipment** ExistingItemPtr = EquipmentMap.Find(ItemClassTag))
// 	{
// 		ULaylaEquipment* ExistingItem = *ExistingItemPtr;
// 		// Is Same Equipment Class? Shotgun & Rifle have same ItemClassTag, but is different equipment(weapon)
// 		if(ItemClass == ExistingItem->GetClass())
// 		{
// 			if(ExistingItem->IsA(ULaylaWeapon::StaticClass())) 			// Same Weapon Class Process Logic: Fill in Ammos?  
// 			{
// 				// CurrentWeapon->FillAmmos?
// 			}	
// 			else 			// Same Other Equipment Class Process Logic...
// 			{
// 				
// 			}
// 		}
// 		// Existing Same Tag but Different Class:
// 		else
// 		{
// 			ExistingItem->OnUnequipped();
//
// 			if(ExistingItem == CurrentWeapon)
// 			{
// 				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
// 			}
// 			// Replace ExistingItem with new ItenInstance
// 			EquipmentMap[ItemClassTag] = ItemInstance;
// 			ItemInstance->OnEquipped();
// 		}
// 	}
// 	// No Existing Same Tag Equipment:
// 	else
// 	{
// 		EquipmentMap.Add(ItemClassTag, ItemInstance);
// 		if(ItemInstance->IsA(ULaylaWeapon::StaticClass())) // if Weapon Class: deal with CurrentWeapon Switch Logic
// 		{
// 			
// 			if(ItemClassTag == "PrimaryWeapon")  // Primary Weapon: Always be CurrentWeapon
// 			{
// 				if(CurrentWeapon){CurrentWeapon->bHandHeld = false;}
// 				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
// 				CurrentWeapon->OnEquipped();
//
// 				CurrentWeapon->bHandHeld = true;
// 				// Client will automatically call OnRep when Property Replicated
// 				// Server manually call OnRep
// 				CurrentWeapon->OnRep_HandHeld(false);
// 				
// 			}else if(ItemClassTag == "SecondaryWeapon" &&  // Secondary Weapon: Only Be CurrentWeapon when No Currrent Weapon or Current Weapon is Melee
// 				( !CurrentWeapon  ||  CurrentWeapon->EquipmentTypeString == "MeleeWeapon"))
// 			{
// 				if(CurrentWeapon){CurrentWeapon->bHandHeld = false;CurrentWeapon->OnRep_HandHeld(true);}
// 				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
// 				CurrentWeapon->OnEquipped();
//
// 				CurrentWeapon->bHandHeld = true;
// 				CurrentWeapon->OnRep_HandHeld(false);
// 				
// 			}else if(ItemClassTag == "MeleeWeapon" &&  !CurrentWeapon) // Melee Weapon: Only be CurrentWeapon when No Current Weapon
// 			{
// 				CurrentWeapon = Cast<ULaylaWeapon>(ItemInstance);
// 				CurrentWeapon->OnEquipped();
// 				
// 				CurrentWeapon->bHandHeld = true;
// 				CurrentWeapon->OnRep_HandHeld(false);
// 			}
// 		}else
// 		{
// 			// Equip new ItemInstance
// 			ItemInstance->OnEquipped();
// 		}
// 	}
// }