// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaWeapon.h"

#include "GameplayTagContainer.h"
#include "LaylaProjectile.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


ULaylaWeapon::ULaylaWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer){}


float ULaylaWeapon::PlayCharacterMontage(UAnimMontage* MontageToPlay) const
{
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && MontageToPlay)
		{
			return Character->PlayAnimMontage(MontageToPlay);
		}
	}
	return 0.f;
}

float ULaylaWeapon::PlayWeaponMontage(UAnimMontage* MontageToPlay) const
{
	for (auto SpawnedWeaponAvator : HandHeldActors)
	{
		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent());
		if( SM && MontageToPlay)
		{
			return SM->GetAnimInstance()->Montage_Play(MontageToPlay);
		}
	}
	return 0.f;
}

void ULaylaWeapon::StopCharacterMontage(UAnimMontage* MontageToStop) const
{
	ACharacter* Character = Cast<ACharacter>(GetTypedPawn(ACharacter::StaticClass()));
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && MontageToStop)
		{
			Character->StopAnimMontage(MontageToStop);
		}
	}
}

void ULaylaWeapon::StopWeaponMontage(UAnimMontage* MontageToStop) const
{
	for (auto SpawnedWeaponAvator : HandHeldActors)
	{
		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(SpawnedWeaponAvator->GetRootComponent());
		if( SM && MontageToStop)
		{
			return SM->GetAnimInstance()->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
		}
	}
}




void ULaylaWeapon::OnRep_HandHeld(bool bPrevHandHeld)
{
	if(bPrevHandHeld == bHandHeld) return ;
	
	// Toggle HandHeld & Wearable Visibility
	for(auto HandHeld: HandHeldActors)
	{
		HandHeld->SetHidden(!bHandHeld);
	}
	for(auto Wearable : SpawnedActors)
	{
		Wearable->SetHidden(bHandHeld);
	}
	
	LinkOwnerPawnToAnimSet();

	float PendingTime = PlayCharacterMontage(
		bHandHeld ? CharacterDrawingMontage : CharacterStowingMontage);

	bPendingDrawingOrStowing = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Pending, this,  &ULaylaWeapon::SetPendingToFalse, PendingTime, false);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_HandHeldAudio, this,  &ULaylaWeapon::PlayHandHeldAudio, PendingTime, false);
}



void ULaylaWeapon::LinkOwnerPawnToAnimSet() const
{
	check(GetTypedPawn(ACharacter::StaticClass()) != nullptr)
	
	if(ACharacter* Character = Cast<ACharacter>(GetPawn()))
	{
		if(Character->GetMesh())
		{
			Character->GetMesh()->LinkAnimClassLayers(bHandHeld ? HandheldAnimSet : NonHandHeldAnimSet);		
		}
	}
}

void ULaylaWeapon::SetLastAttackTime()
{
	LastAttackTime = GetWorld()->GetTimeSeconds();
}


void ULaylaWeapon::SetPendingToFalse()
{
	bPendingDrawingOrStowing = false;
}

void ULaylaWeapon::PlayHandHeldAudio()
{
	// Play Some Audio to indicate drawing is over
}

// void ULaylaWeapon::ServerStopFire_Implementation()
// {
// 	bIsFiring = false;
// }
//
// bool ULaylaWeapon::ServerStopFire_Validate()
// {
// 	return true;
// }
//
//
// void ULaylaWeapon::ServerWeaponReload_Implementation()
// {
// 	int ReloadAmount = FMath::Min(MagazineSize, CurrAmmoInBackpack);
// 	CurrAmmoInMagazine += ReloadAmount;  
// 	CurrAmmoInBackpack -= ReloadAmount;
// 	
// }
//
// bool ULaylaWeapon::ServerWeaponReload_Validate()
// {
// 	return CurrAmmoInBackpack>0 && MagazineSize>0;
// }
//
// void ULaylaWeapon::ServerGenerateProjectile_Implementation()
// {
// 	FVector SpawnLocation = *CameraLocation + ((*CameraRotation).Vector() * 200.0f);
// 	// FRotator spawnRotation = ShootDirection.Rotation();
// 	
// 	FActorSpawnParameters spawnParameters;
// 	spawnParameters.Instigator = GetPawn();
// 	spawnParameters.Owner = GetPawn()->GetOwner();
// 	ALaylaProjectile* Projectile = GetWorld()->SpawnActor<ALaylaProjectile>(ProjectileClass, SpawnLocation, *CameraRotation, spawnParameters);
// 	if(Projectile)
// 	{
// 		Projectile->FireInDirection( (*CameraRotation).Vector());
// 	}
// 	CurrAmmoInMagazine -= 1;
// }
//
// bool ULaylaWeapon::ServerGenerateProjectile_Validate()
// {
// 	return CurrAmmoInMagazine>0 && bIsRangedWeapon;
// }


void ULaylaWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, HandHeldActors);
	DOREPLIFETIME(ThisClass, bAttacking);
	DOREPLIFETIME(ThisClass, bHandHeld);
}

void ULaylaWeapon::OnEquipped()
{
	Super::OnEquipped();

	// Spawn HandHeld Actors
	SpawnEquipmentActors(DefaultHandHeldActorsToSpawn, HandHeldActors);
	for(auto HandHeldActor : HandHeldActors)
	{
		HandHeldActor->SetHidden(true);
	}
}

void ULaylaWeapon::OnUnequipped()
{
	Super::OnUnequipped();

	// Destroy Spawned Extra HandHeld Actors
	DestroyEquipmentActors(HandHeldActors);
}

void ULaylaWeapon::StartAttack()
{
	if(GetPawn()->GetLocalRole() < ROLE_Authority)
	{
		ServerStartAttack();
	}

	if(!bAttacking && GetWorld()->GetTimeSeconds() - LastAttackTime > CoolingTime)
	{
		bAttacking = true;
		OnRep_Attacking(false);
	}
}

void ULaylaWeapon::OnRep_Attacking(bool bPrevAttacking)
{
	if(bPrevAttacking == false && bAttacking == true) // Start Firing
	{
		float ChMontageTime = PlayCharacterMontage(CharacterAttackMontage);
		float WPMontageTime = PlayWeaponMontage(WeaponAttackMontage);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CharacterAttackMontage,
			[this](){PlayCharacterMontage(CharacterAttackMontage);}, ChMontageTime, true);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_WeaponAttackMontage,
			[this](){PlayWeaponMontage(WeaponAttackMontage);}, WPMontageTime, true);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SetLastAttackTime,
			this, &ULaylaWeapon::SetLastAttackTime,CoolingTime, true);
	}
	else if(bPrevAttacking == true && bAttacking == false) // Cease  Firing
	{
		StopCharacterMontage(CharacterAttackMontage);
		StopWeaponMontage(WeaponAttackMontage);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CharacterAttackMontage);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_WeaponAttackMontage);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SetLastAttackTime);
	}
}


void ULaylaWeapon::StopAttack()
{
	if(GetPawn()->GetLocalRole() < ROLE_Authority)
	{
		ServerStartAttack();
	}

	if(bAttacking)
	{
		bAttacking = false;
		OnRep_Attacking(true);
	}
	
}

bool ULaylaWeapon::ServerStopAttack_Validate()
{
	return true;
}

bool ULaylaWeapon::ServerStartAttack_Validate()
{
	return true;
}

void ULaylaWeapon::ServerStopAttack_Implementation()
{
	StopAttack();
}

void ULaylaWeapon::ServerStartAttack_Implementation()
{
	StartAttack();
}



