// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaWeapon_Ranged.h"

//
// void ULaylaWeapon::Reload()
// {
// 	if(bIsReloading) return;
//
//
// 	bIsReloading = true;
// 	if(GetPawn() && GetPawn()->GetOwner()->HasAuthority())
// 	{
// 		// Delay time must match Montage Length
// 		float WeaponReloadTime = 1.0f;
// 		GetWorld()->GetTimerManager().SetTimer(WeaponReloadTimer, this, &ULaylaWeapon::ServerWeaponReload, WeaponReloadTime, false);
// 		GetWorld()->GetTimerManager().SetTimer(WeaponReloadResetTimer, this, &ULaylaWeapon::ResetReloadState, WeaponReloadTime, false);
// 	}
// 	
// 	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
// 	if(Character)
// 	{
// 		USkeletalMeshComponent* Mesh = Character->GetMesh();
// 		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
// 		if(AnimInstance && CharacterReloadMontage)
// 		{
// 			Character->PlayAnimMontage(CharacterReloadMontage);
// 		}
// 	}
// 	
// 	for (auto SpawnedWeaponAvator : GetSpawnedActors())
// 	{
// 		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent());
// 		if( SM && WeaponReloadMontage)
// 		{
// 			SM->GetAnimInstance()->Montage_Play(WeaponReloadMontage);
// 		}
// 	}
// }
//
//
//
//
// void ULaylaWeapon::StartFire(FVector* Location, FRotator* Rotation)
// {
// 	// 连点也不能超过开火速率
// 	if(bIsFiring || GetWorld()->GetTimeSeconds() - LastFireTime <= FireRate) return;
// 	
// 	CameraLocation = Location;
// 	CameraRotation = Rotation;
// 	
// 	// Register Montage Loop
// 	PlayFireMontage();
// 	GetWorld()->GetTimerManager().SetTimer(PlayMontageTimer, this, &ULaylaWeapon::PlayFireMontage, FireRate, true);
// 	
// 	// Generate Projectile is RPC on Server
// 	if(GetPawn() && GetPawn()->IsLocallyControlled())
// 	{
// 		
// 		// Register Montage Loop
// 		// Generate One Bullet Immediately
// 		ServerGenerateProjectile();
// 		GetWorld()->GetTimerManager().SetTimer(GenerateProjectileTimer, this, &ULaylaWeapon::ServerGenerateProjectile, FireRate, true);
// 	}
//
// 	bIsFiring = true;
// }
//
// void ULaylaWeapon::StopFire()
// {
// 	bIsFiring = false;
// 	GetWorld()->GetTimerManager().ClearTimer(PlayMontageTimer);
// 	GetWorld()->GetTimerManager().ClearTimer(GenerateProjectileTimer);
//
// 	
// }