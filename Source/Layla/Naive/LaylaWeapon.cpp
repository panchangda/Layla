// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaWeapon.h"

#include "GameplayTagContainer.h"
#include "LaylaProjectile.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


ULaylaWeapon::ULaylaWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}



void ULaylaWeapon::StartFire(const FVector& Location, const FRotator& Rotation)
{
	// 连点也不能超过开火速率
	if(bIsFiring || GetWorld()->GetTimeSeconds() - LastFireTime <= FireRate) return;
	
	CameraLocation = Location;
	CameraRotation = Rotation;
	
	// Register Montage Loop
	PlayFireMontage();
	GetWorld()->GetTimerManager().SetTimer(PlayMontageTimer, this, &ULaylaWeapon::PlayFireMontage, FireRate, true);
	
	// Generate Projectile is RPC on Server
	if(GetPawn() && GetPawn()->IsLocallyControlled())
	{
		
		// Register Montage Loop
		// Generate One Bullet Immediately
		ServerGenerateProjectile();
		GetWorld()->GetTimerManager().SetTimer(GenerateProjectileTimer, this, &ULaylaWeapon::ServerGenerateProjectile, FireRate, true);
	}

	bIsFiring = true;
}

void ULaylaWeapon::StopFire()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(PlayMontageTimer);
	GetWorld()->GetTimerManager().ClearTimer(GenerateProjectileTimer);
}

void ULaylaWeapon::PlayFireMontage()
{
	LastFireTime = GetWorld()->GetTimeSeconds();
	if(CurrAmmoInMagazine<=0) return ;
	
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
		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent());
		if( SM && WeaponFireMontage)
		{
			SM->GetAnimInstance()->Montage_Play(WeaponFireMontage);
		}
	}
}

void ULaylaWeapon::Reload()
{
	if(bIsReloading) return;


	bIsReloading = true;
	if(GetPawn() && GetPawn()->GetOwner()->HasAuthority())
	{
		// Delay time must match Montage Length
		float WeaponReloadTime = 1.0f;
		GetWorld()->GetTimerManager().SetTimer(WeaponReloadTimer, this, &ULaylaWeapon::ServerWeaponReload, WeaponReloadTime, false);
		GetWorld()->GetTimerManager().SetTimer(WeaponReloadResetTimer, this, &ULaylaWeapon::ResetReloadState, WeaponReloadTime, false);
	}
	
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
		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent());
		if( SM && WeaponReloadMontage)
		{
			SM->GetAnimInstance()->Montage_Play(WeaponReloadMontage);
		}
	}
}

void ULaylaWeapon::ResetReloadState()
{
	bIsReloading = false;
}

void ULaylaWeapon::ServerWeaponReload_Implementation()
{
	int ReloadAmount = FMath::Min(MagazineSize, CurrAmmoInBackpack);
	CurrAmmoInMagazine += ReloadAmount;  
	CurrAmmoInBackpack -= ReloadAmount;
}

bool ULaylaWeapon::ServerWeaponReload_Validate()
{
	return CurrAmmoInBackpack>0 && MagazineSize>0;
}

void ULaylaWeapon::ServerGenerateProjectile_Implementation()
{
	FVector SpawnLocation = CameraLocation + (CameraRotation.Vector() * 200.0f);
	// FRotator spawnRotation = ShootDirection.Rotation();
	
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetPawn();
	spawnParameters.Owner = GetPawn()->GetOwner();
	ALaylaProjectile* Projectile = GetWorld()->SpawnActor<ALaylaProjectile>(ProjectileClass, SpawnLocation, CameraRotation, spawnParameters);
	if(Projectile)
	{
		Projectile->FireInDirection(CameraRotation.Vector());
	}
	CurrAmmoInMagazine -= 1;
}

bool ULaylaWeapon::ServerGenerateProjectile_Validate()
{
	return CurrAmmoInMagazine>0 && bIsRangedWeapon;
}


void ULaylaWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrAmmoInBackpack)
	DOREPLIFETIME(ThisClass, CurrAmmoInMagazine);
}

void ULaylaWeapon::OnEquipped()
{
	Super::OnEquipped();

	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(false, FGameplayTagContainer()));
	}
}

void ULaylaWeapon::OnUnequipped()
{
	Super::OnUnequipped();
	
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(false, FGameplayTagContainer()));
	}
}

// void ULaylaWeapon::OnEquipped_Implementation()
// {
// 	Super::OnEquipped_Implementation();
//
// 	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
// 	if(Character)
// 	{
// 		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(false, FGameplayTagContainer()));
// 	}
// }

// void ULaylaWeapon::OnUnequipped_Implementation()
// {
// 	Super::OnUnequipped_Implementation();
//
// 	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
// 	if(Character)
// 	{
// 		Character->GetMesh()->LinkAnimClassLayers(PickBestAnimLayer(false, FGameplayTagContainer()));
// 	}
// }
//

TSubclassOf<UAnimInstance> ULaylaWeapon::PickBestAnimLayer(bool bEquipped,
                                                           const FGameplayTagContainer& CosmeticTags) const
{
	const FLaylaAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
	return SetToQuery.DefaultLayer;
}

