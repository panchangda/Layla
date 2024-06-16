// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaWeapon.h"

#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"

ULaylaWeapon::ULaylaWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ULaylaWeapon::OnEquipped()
{
	Super::OnEquipped();
	
	SpawnEquipmentActors({DefaultActorsToSpawn});
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(true, FGameplayTagContainer()));
	}
}

void ULaylaWeapon::OnUnequipped()
{
	Super::OnUnequipped();

	DestroyEquipmentActors();
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(false, FGameplayTagContainer()));
	}
	
}

void ULaylaWeapon::Fire()
{
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && CharacterFireMontage)
		{
			Character->PlayAnimMontage(CharacterFireMontage);
		}
	}
	
	for (auto SpawnedWeaponAvator : GetSpawnedActors())
	{
		if(USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent()))
		{
			SM->GetAnimInstance()->Montage_Play(WeaponFireMontage);
		}
	}
	
	GenerateBullet();
}

void ULaylaWeapon::Reload()
{
	
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && CharacterReloadMontage)
		{
			Character->PlayAnimMontage(CharacterReloadMontage);
		}
	}
	
	for (auto SpawnedWeaponAvator : GetSpawnedActors())
	{
		if(USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent()))
		{
			SM->GetAnimInstance()->Montage_Play(WeaponReloadMontage);
		}
	}
}

TSubclassOf<UAnimInstance> ULaylaWeapon::PickBestAnimLayer(bool bEquipped,
                                                           const FGameplayTagContainer& CosmeticTags) const
{
	const FLaylaAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
	return SetToQuery.DefaultLayer;
}


void ULaylaWeapon::GenerateBullet_Implementation()
{
	
}


