// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaylaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "VectorTypes.h"
#include "../LaylaEquipmentManager.h"
#include "../LaylaPickup.h"
#include "../Weapon/LaylaWeapon.h"
#include "Layla/LaylaGun.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALaylaCharacter

bool ALaylaCharacter::LineTraceAlongCamera(FHitResult& HitResult)
{

	FVector CameraViewNearEnd = CameraLocation + CameraRotation.Vector() * 500.0f;
	FVector CameraViewFarEnd = CameraLocation + CameraRotation.Vector() * 20000000.0f;

	
	FVector SpawnLocation = FVector(0.0, 0.0, 0.0);
	FRotator SpawnRotation = (CameraViewFarEnd - SpawnLocation).Rotation();	
	DrawDebugLine(GetWorld(), CameraLocation, CameraViewFarEnd,  FColor::Red);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	return GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraViewFarEnd, ECC_Visibility, QueryParams);
}

ALaylaCharacter::ALaylaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(100.0f, 50.0f, 60.0f); // Relatively move end of spring arm, for CrossHair and so on 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	// Initialize EquipmentManager
	// EquipmentManager = CreateDefaultSubobject<ULaylaEquipmentManager>(TEXT("EquipmentManager"));
	


	// bReplicateUsingRegisteredSubObjectList = false;
}

void ALaylaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ALaylaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Update Came Info
	CameraLocation = FollowCamera->GetComponentLocation();;
	// CameraRotation = GetController()->GetControlRotation(); Error when multi player???
	CameraRotation = FollowCamera->GetComponentRotation();

	// Try Trace Interactable Objects
	FHitResult HitResult;
	if (LineTraceAlongCamera(HitResult))
	{
		AActor* HitActor = HitResult.GetActor();
		// 检查是否是我们感兴趣的目标类型
		if (HitActor->IsA(ALaylaPickup::StaticClass()))
		{
			// 显示UI
			ALaylaPickup* PickupActor = Cast<ALaylaPickup>(HitActor);
			if(PickupActor && UE::Geometry::Distance(PickupActor->GetActorLocation(), GetActorLocation()) < 300.0f)
			{
				PickupActor->bIsBeingInspected = true;
			}
		}else if(HitActor->IsA(ALaylaGun::StaticClass()))
		{
			// 显示UI
			ALaylaGun* GunActor = Cast<ALaylaGun>(HitActor);
			if(UE::Geometry::Distance(GunActor->GetActorLocation(), GetActorLocation()) < 300.0f)
			{
				GunActor->OnFocused();
			}
		}

		// 如果和上次求交的actor不同
		if(HitActor!=PrevHitActor)
		{
			if(ALaylaGun* PrevGunActor = Cast<ALaylaGun>(PrevHitActor))
			{
				PrevGunActor->OnFocuseLost();
			}

			PrevHitActor = HitActor;
		}
	}
	else // 求交不成功
	{
		if(ALaylaGun* GunActor = Cast<ALaylaGun>(PrevHitActor))
		{
			GunActor->OnFocuseLost();
		}

		
		PrevHitActor = nullptr;
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void ALaylaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::Look);
		
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::UpdateCrouchState);
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::Interact);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::Aim);
		
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ALaylaCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ALaylaCharacter::StopFire);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::Reload);


		// Equip
		EnhancedInputComponent->BindAction(EquipPrimaryAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::EquipPrimaryWeapon);
		EnhancedInputComponent->BindAction(EquipSecondaryAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::EquipSecondaryWeapon);
		EnhancedInputComponent->BindAction(EquipMeleeAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::EquipMeleeWeapon);
		EnhancedInputComponent->BindAction(DropGunAction, ETriggerEvent::Triggered, this, &ALaylaCharacter::DropGun);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALaylaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = GetMaxHealth();

		// // Needs to happen after character is added to repgraph
		// GetWorldTimerManager().SetTimerForNextTick(this->EquipmentManager, &ULaylaEquipmentManager::SpawnDefaultEquipment);
	}
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		// if (RespawnFX)
		// {
		// 	UGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, GetActorLocation(), GetActorRotation());
		// }
		//
		// if (RespawnSound)
		// {
		// 	UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		// }
	}
	
}


void ALaylaCharacter::PickGun(ALaylaGun* Gun)
{
	if (Gun)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			AddGun(Gun);
		}
		else
		{
			ServerPickGun(Gun);
		}
	}
}

void ALaylaCharacter::DropGun(ALaylaGun* Gun)
{
	if (Gun)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			RemoveGun(Gun);
		}
		else
		{
			ServerDropGun(Gun);
		}
	}
}

ALaylaGun* ALaylaCharacter::FindGun(const FGameplayTag& GunToFind)
{
	for(const auto Gun:GunList)
	{
		if(Gun->GunTag == GunToFind)
		{
			return Gun;
		}
	}
	return nullptr;
}

void ALaylaCharacter::SetCurrentGun(ALaylaGun* NewGun, ALaylaGun* LastGun)
{
	ALaylaGun* LocalLastWeapon = nullptr;

	if (LastGun != NULL)
	{
		LocalLastWeapon = LastGun;
	}
	else if (NewGun != CurrentGun)
	{
		LocalLastWeapon = CurrentGun;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentGun = NewGun;

	// equip new one
	if (NewGun)
	{
		NewGun->SetOwnerPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewGun->OnEquip();
	}
	
}
void ALaylaCharacter::OnRep_CurrentGun(ALaylaGun* LastGun)
{
	SetCurrentGun(CurrentGun, LastGun);
}


void ALaylaCharacter::EquipGun_Implementation(ALaylaGun* Gun)
{
	if(Gun)
	{
		SetCurrentGun(Gun, CurrentGun);
	}
}

bool ALaylaCharacter::EquipGun_Validate(ALaylaGun* Gun)
{
	return true;
}


void ALaylaCharacter::UnEquipGun_Implementation(ALaylaGun* Gun)
{

}

bool ALaylaCharacter::UnEquipGun_Validate(ALaylaGun* Gun)
{
	return true;
}


void ALaylaCharacter::ServerDropGun_Implementation(ALaylaGun* Gun)
{
	DropGun(Gun);
}

bool ALaylaCharacter::ServerDropGun_Validate(ALaylaGun* Gun)
{
	return true;
}

void ALaylaCharacter::ServerPickGun_Implementation(ALaylaGun* Gun)
{
	PickGun(Gun);
}

bool ALaylaCharacter::ServerPickGun_Validate(ALaylaGun* Gun)
{
	return true;
}


void ALaylaCharacter::AddGun(ALaylaGun* Gun)
{
	ALaylaGun* SameTagGun = FindGun(Gun->GunTag);
	GunList.AddUnique(Gun);
	Gun->OnPick(this);
	if(SameTagGun)
	{
		RemoveGun(SameTagGun);
	}else
	{
		EquipGun(Gun);
	}
}

void ALaylaCharacter::RemoveGun(ALaylaGun* Gun)
{
	if(Gun == CurrentGun)
	{
		SetCurrentGun(nullptr, Gun);
	}
	GunList.RemoveSingle(Gun);
	Gun->OnDrop(this);
}

void ALaylaCharacter::StartGunFire()
{
	if (!bIsGunFiring)
	{
		bIsGunFiring = true;
		if (CurrentGun)
		{
			CurrentGun->StartFire();
		}
	}
}

void ALaylaCharacter::StopGunFire()
{
	if(bIsGunFiring)
	{
		bIsGunFiring = false;
		if(CurrentGun)
		{
			CurrentGun->StopFire();
		}
	}
}

void ALaylaCharacter::StartGunReload()
{
	if (CurrentGun)
	{
		CurrentGun->StartReload();
	}
}

void ALaylaCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALaylaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ALaylaCharacter::Reload(const FInputActionValue& Value)
{
	// EquipmentManager->GetCurrentWeapon()->Reload();
}

void ALaylaCharacter::EquipPrimaryWeapon(const FInputActionValue& Value)
{
	// EquipmentManager->HandleInputToEquip(EEquipmentInput::EquipPrimary);
	EquipGun(FindGun(FGameplayTag::RequestGameplayTag(FName("Weapon.Gun.Primary"))));
}

void ALaylaCharacter::EquipSecondaryWeapon(const FInputActionValue& Value)
{
	// EquipmentManager->HandleInputToEquip(EEquipmentInput::EquipSecondary);
	EquipGun(FindGun(FGameplayTag::RequestGameplayTag(FName("Weapon.Gun.Secondary"))));
}

void ALaylaCharacter::EquipMeleeWeapon(const FInputActionValue& Value)
{
	// EquipmentManager->HandleInputToEquip(EEquipmentInput::EquipMelee);
	EquipGun(FindGun(FGameplayTag::RequestGameplayTag(FName("Weapon.Melee"))));
}

void ALaylaCharacter::DropGun(const FInputActionValue& Value)
{
		DropGun(CurrentGun);
}


// bool ALaylaCharacter::EquipWeapon_Validate(const FString& WeaponType)
// {
// 	return WeaponType.Equals("PrimaryWeapon")
// 	|| WeaponType.Equals("SecondaryWeapon")
// 	|| WeaponType.Equals("MeleeWeapon");
// }

void ALaylaCharacter::UpdateCrouchState(const FInputActionValue& Value)
{
	bIsCrouched = !bIsCrouched;
}

// Interact with 
void ALaylaCharacter::Interact(const FInputActionValue& Value)
{
	FHitResult HitResult;
	if (LineTraceAlongCamera(HitResult))
	{
		AActor* HitActor = HitResult.GetActor();
		// 检查是否是我们感兴趣的目标类型
		// if (HitActor->IsA(ALaylaPickup::StaticClass()))
		// {
		// 	// 显示UI
		// 	ALaylaPickup* PickupActor = Cast<ALaylaPickup>(HitActor);
		// 	if(PickupActor && UE::Geometry::Distance(PickupActor->GetActorLocation(), GetActorLocation()) < 300.0f)
		// 	{
		//
		// 		EquipmentManager->AddItem(PickupActor->EquipmentClass);
		// 		PickupActor->OnPickup();
		// 		
		// 	}
		// }
		if(HitActor->IsA(ALaylaGun::StaticClass()))
		{
			ALaylaGun* GunActor = Cast<ALaylaGun>(HitActor);
			PickGun(GunActor);
		}
	}
	
}

void ALaylaCharacter::Aim(const FInputActionValue& Value)
{
	// haven't started aiming 
	if(!isAiming)
	{
		CameraBoom->TargetArmLength = 250.0f;
		CameraBoom->SocketOffset = FVector(100.0f, 45.0f, 70.0f); // Relatively move end of spring arm, for CrossHair and so on
		FollowCamera->SetFieldOfView(70.0f);
	}
	else // already at aiming state 
	{
		CameraBoom->TargetArmLength = 400.0f;
		CameraBoom->SocketOffset = FVector(100.0f, 50.0f, 60.0f); // Relatively move end of spring arm, for CrossHair and so on
		FollowCamera->SetFieldOfView(90.0f);
	}
	isAiming = !isAiming;
}

void ALaylaCharacter::StartFire(const FInputActionValue& Value)
{
	// EquipmentManager->HandleInputToEquip(EEquipmentInput::StartAttack);
	// EquipmentManager->GetCurrentWeapon()->StartFire(&CameraLocation, &CameraRotation);
}

void ALaylaCharacter::StopFire(const FInputActionValue& Value)
{
	// EquipmentManager->HandleInputToEquip(EEquipmentInput::StopAttack);
	// EquipmentManager->GetCurrentWeapon()->StopFire();
}




// Network Replication related funcs
void ALaylaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//复制当前生命值。
	DOREPLIFETIME(ALaylaCharacter, CurrentHealth);
	DOREPLIFETIME(ALaylaCharacter, GunList);
	DOREPLIFETIME(ALaylaCharacter, CurrentGun);
}

// Server-Side Set
void ALaylaCharacter::SetCurrentHealth(float healthValue)
{
	check(GetLocalRole() == ROLE_Authority)
	
	CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
	OnHealthUpdate();

}

float ALaylaCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{

	// Only Server should take damage: See APawn::ShouldTakeDamage
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if(ActualDamage!= 0.f)
	{
		SetCurrentHealth(CurrentHealth - ActualDamage);
	}
	
	return ActualDamage;
}



void ALaylaCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ALaylaCharacter::OnHealthUpdate()
{
	//客户端特定的功能
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
 
		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
			
		}
	}
 
	//服务器特定的功能
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
 
	//在所有机器上都执行的函数。
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
}