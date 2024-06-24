// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGun.h"

#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


// Sets default values
ALaylaGun::ALaylaGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create the Static Mesh Component.
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	// Set the Static Mesh Component as the Root Component.
	RootComponent = SkeletalMeshComponent;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);

	WidgetComponent->SetVisibility(false);

	this->SetReplicates(true);
	SkeletalMeshComponent->SetIsReplicated(true);
}

void ALaylaGun::OnFocused()
{
	WidgetComponent->SetVisibility(true);
}

void ALaylaGun::OnFocuseLost()
{
	WidgetComponent->SetVisibility(false);
}

void ALaylaGun::OnDrop(APawn* PrevOwner)
{
	
	if(bIsEquipped)
	{
		LinkPawnToAnimSet(UnArmedAnimSet, PrevOwner);
		bIsEquipped = false;
	}else if(bPendingEquip) // Stop Pending Animation, Clear PendingEquip Timer
	{
		StopCharacterAnim(CharacterEquipAnim);
		LinkPawnToAnimSet(UnArmedAnimSet, PrevOwner);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
		bIsEquipped = false;
		bPendingEquip = false;
	}
	
	DetachFromPawn();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		
		SetOwnerPawn(nullptr);
	}

	// Drop Action: Simulate parabola
	FVector OwnerLocation = PrevOwner->GetActorLocation();
	FVector OwnerForwardV = PrevOwner->GetActorForwardVector() * 100.0f; // Adjust the multiplier as needed
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkeletalMeshComponent->SetWorldLocation(OwnerLocation + OwnerForwardV);
	// SkeletalMeshComponent->SetWorldLocation(OwnerLocation, false, nullptr, ETeleportType::ResetPhysics);
	SkeletalMeshComponent->SetRelativeScale3D(MeshScale);
	
	
}

void ALaylaGun::OnPick(APawn* NewOwner)
{
	SetOwnerPawn(NewOwner);

	AttachToPawn(AttachSocket_Body, AttachTransform_Body);

	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetEnableGravity(false);
	SkeletalMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
}

void ALaylaGun::OnEquip()
{
	bPendingEquip = true;
	AttachToPawn(AttachSocket_Hand, AttachTransform_Hand);
	LinkOwnerPawnToAnimSet(ArmedAnimSet);
	float Duration = PlayCharacterAnim(CharacterEquipAnim);
	if(Duration <= 0.0f)
	{
		// failsafe
		Duration = 0.5f;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ALaylaGun::OnEquipFinished, Duration, false);
}

void ALaylaGun::OnUnEquip()
{
	if(bPendingEquip)
	{
		StopCharacterAnim(CharacterEquipAnim);
	}
	AttachToPawn(AttachSocket_Body, AttachTransform_Body);
	LinkOwnerPawnToAnimSet(UnArmedAnimSet);
	bIsEquipped = false;
}


void ALaylaGun::SetOwnerPawn(APawn* NewOwner)
{
	if(OwnerPawn!= NewOwner)
	{
		SetInstigator(NewOwner);
		OwnerPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(OwnerPawn);
	}
}
void ALaylaGun::OnRep_OwnerPawn(APawn* PrevPawn)
{
	if(OwnerPawn)
	{
		OnPick(OwnerPawn);
	}
	else
	{
		OnDrop(PrevPawn);	
	}
}

APawn* ALaylaGun::GetOwnerPawn()
{
	return OwnerPawn;
}

void ALaylaGun::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ALaylaGun::StopFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
	
}

void ALaylaGun::StartReload(bool bFromReplication)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}
	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		float AnimDuration_Gun = PlayWeaponAnim(GunReloadAnim);
		float AnimDuration_Character = PlayCharacterAnim(CharacterReloadAnim);
		float AnimDuration = FMath::Max(AnimDuration_Gun, AnimDuration_Character);
		if (AnimDuration <= 0.0f)
		{
			// failsafe
			AnimDuration = 1.0f;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ALaylaGun::StopReload, AnimDuration, false);
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ALaylaGun::ReloadGun, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}
		
		if (OwnerPawn && OwnerPawn->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void ALaylaGun::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopCharacterAnim(CharacterReloadAnim);
		StopWeaponAnim(GunReloadAnim);
	}
}

void ALaylaGun::ReloadGun()
{
	AmmoInMagzine = GunConfig.MagazineSize;
}

void ALaylaGun::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

void ALaylaGun::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;
	if(bIsEquipped)
	{
		if( bPendingReload)
		{
			if( CanReload() == false )
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}else if ( (bPendingReload == false ) && ( bWantsToFire == true ) && ( CanFire() == true ))
		{
			NewState = EWeaponState::Firing;
		}
		
	}else if(bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void ALaylaGun::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void ALaylaGun::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (GunConfig.FireRate > 0.0f &&
		LastFireTime + GunConfig.FireRate > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ALaylaGun::HandleFiring, LastFireTime + GunConfig.FireRate - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ALaylaGun::OnBurstFinished()
{
	BurstCounter = 0;
	
	StopSimulatingWeaponFire();

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	
}

void ALaylaGun::SimulateWeaponFire()
{
	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	// Anim      
	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnim(GunFireAnim);
		PlayCharacterAnim(CharacterFireAnim);
		bPlayingFireAnim = true;
	}
	
	// Sound     
	if (bLoopedFireSound)
	{
		if (FireAC == NULL)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}
	
	// Muzzle FX
	
	// Camera Shake


}

void ALaylaGun::StopSimulatingWeaponFire()
{
	if (bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnim(GunFireAnim);
		bPlayingFireAnim = false;
	}
	if(FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}
}

void ALaylaGun::Fire()
{
}

void ALaylaGun::OnRep_Reload()
{
	if(bPendingReload)
	{
		StartReload(true);
	}else
	{
		StopReload();
	}
	
}

bool ALaylaGun::CanFire()
{
	bool bCanFire = OwnerPawn!= nullptr;
	bool bStateOKToFire = ( ( CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing) );
	return (( bCanFire == true ) && ( bStateOKToFire == true ) && ( bPendingReload == false ));
}

bool ALaylaGun::CanReload()
{
	bool bCanReload = OwnerPawn!= nullptr;
	bool bGotAmmo = true; // Get Ammo Count From Character's Inventory List
	bool bAmmoIsNotFull = AmmoInMagzine!=GunConfig.MagazineSize;
	bool bStateOKToReload = ( ( CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing) );
	return bCanReload && bGotAmmo && bAmmoIsNotFull && bStateOKToReload;
}

void ALaylaGun::ServerStopFire_Implementation()
{
	StopFire();
}

bool ALaylaGun::ServerStopFire_Validate()
{
	return true;
}

void ALaylaGun::ServerStartFire_Implementation()
{
	StartFire();
}

bool ALaylaGun::ServerStartFire_Validate()
{
	return true;
}

void ALaylaGun::ServerStartReload_Implementation()
{
	StartReload();
}

bool ALaylaGun::ServerStartReload_Validate()
{
	return true;
}

void ALaylaGun::ServerStopReload_Implementation()
{
	StopReload();
}

bool ALaylaGun::ServerStopReload_Validate()
{
	return true;
}

void ALaylaGun::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = (AmmoInMagzine > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
}

bool ALaylaGun::ServerHandleFiring_Validate()
{
	return true;
}

void ALaylaGun::HandleReFiring()
{
	// Update TimerIntervalAdjustment
	UWorld* MyWorld = GetWorld();

	float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) - GunConfig.FireRate);

	if (bAllowAutomaticWeaponCatchup)
	{
		TimerIntervalAdjustment -= SlackTimeThisFrame;
	}

	HandleFiring();
}

void ALaylaGun::UseAmmo()
{
	AmmoInMagzine--;
}

void ALaylaGun::AttachToPawn(FName Socket, FTransform Transform)
{
	if(OwnerPawn)
	{
		USceneComponent* AttachTarget = OwnerPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwnerPawn))
		{
			AttachTarget = Char->GetMesh();
		}
		// SkeletalMeshComponent->SetRelativeTransform(Transform);
		SkeletalMeshComponent->SetRelativeTransform(Transform);
		SkeletalMeshComponent->AttachToComponent(AttachTarget,FAttachmentTransformRules::KeepRelativeTransform, Socket);
	}
}



void ALaylaGun::DetachFromPawn()
{
	SkeletalMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

void ALaylaGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALaylaGun, OwnerPawn)
	
	DOREPLIFETIME_CONDITION( ALaylaGun, AmmoInMagzine,		COND_OwnerOnly );
	// DOREPLIFETIME_CONDITION( ALaylaGun, CurrentAmmoInClip, COND_OwnerOnly );

	DOREPLIFETIME_CONDITION( ALaylaGun, BurstCounter,		COND_SkipOwner );
	DOREPLIFETIME_CONDITION( ALaylaGun, bPendingReload,		COND_SkipOwner );
}

float ALaylaGun::PlayCharacterAnim(UAnimMontage* AnimToPlay)
{
	ACharacter* Character = Cast<ACharacter>(OwnerPawn);
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && AnimToPlay)
		{
			return Character->PlayAnimMontage(AnimToPlay);
		}
	}
	return 0.f;
}

float ALaylaGun::PlayWeaponAnim(UAnimMontage* AnimToPlay)
{
	UAnimInstance* AnimInstance = SkeletalMeshComponent ? SkeletalMeshComponent->GetAnimInstance() : nullptr;
	if (AnimInstance && AnimToPlay)
	{
		return AnimInstance->Montage_Play(AnimToPlay);
	}

	return 0.f;
}

void ALaylaGun::StopCharacterAnim(UAnimMontage* AnimToStop)
{
	ACharacter* Character = Cast<ACharacter>(OwnerPawn);
	if(Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
		if(AnimInstance && AnimToStop)
		{
			Character->StopAnimMontage(AnimToStop);
		}
	}
}

void ALaylaGun::StopWeaponAnim(UAnimMontage* AnimToStop)
{
	UAnimInstance* AnimInstance = SkeletalMeshComponent ? SkeletalMeshComponent->GetAnimInstance() : nullptr;
	if (AnimInstance && AnimToStop)
	{
		AnimInstance->Montage_Stop(0.0f, AnimToStop);
	}
}

void ALaylaGun::LinkOwnerPawnToAnimSet(TSubclassOf<UAnimInstance> AnimSetToLink) const
{
	if (ACharacter* Character = Cast<ACharacter>(OwnerPawn))
	{
		if (Character->GetMesh())
		{
			Character->GetMesh()->LinkAnimClassLayers(AnimSetToLink);
		}
	}
}

void ALaylaGun::LinkPawnToAnimSet(TSubclassOf<UAnimInstance> AnimSetToLink, APawn* Pawn) const
{
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (Character->GetMesh())
		{
			Character->GetMesh()->LinkAnimClassLayers(AnimSetToLink);
		}
	}
}

UAudioComponent* ALaylaGun::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && OwnerPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, OwnerPawn->GetRootComponent());
	}

	return AC;
}

// Called when the game starts or when spawned
void ALaylaGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaylaGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaylaGun::HandleFiring()
{
	if(AmmoInMagzine > 0 && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (OwnerPawn && OwnerPawn->IsLocallyControlled())
		{
			Fire();

			UseAmmo();
			
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}else if(OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		if(AmmoInMagzine == 0)
		{ 
			PlayWeaponSound(OutOfAmmoSound);
		}

		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
		
	}else
	{
		OnBurstFinished();
	}

	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && GunConfig.FireRate > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this,
				&ALaylaGun::HandleReFiring, FMath::Max<float>(GunConfig.FireRate + TimerIntervalAdjustment, SMALL_NUMBER), false);
			TimerIntervalAdjustment = 0.f;
		}
	}
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ALaylaGun::OnEquipFinished()
{
	bIsEquipped = true;
	bPendingEquip = false;
}

