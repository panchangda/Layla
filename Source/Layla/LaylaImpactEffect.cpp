// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaylaImpactEffect.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#define LAYLA_SURFACE_Default	SurfaceType_Default
#define LAYLA_SURFACE_Flesh		SurfaceType1
#define LAYLA_SURFACE_Dirt		SurfaceType2
#define LAYLA_SURFACE_Water		SurfaceType3
#define LAYLA_SURFACE_Metal		SurfaceType4
#define LAYLA_SURFACE_Wood		SurfaceType5
#define LAYLA_SURFACE_Grass		SurfaceType6
#define LAYLA_SURFACE_Glass		SurfaceType7
#define LAYLA_SURFACE_Concrete	SurfaceType8


ALaylaImpactEffect::ALaylaImpactEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetAutoDestroyWhenFinished(true);
}

void ALaylaImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* ALaylaImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
		case LAYLA_SURFACE_Concrete:	ImpactFX = ConcreteFX; break;
		case LAYLA_SURFACE_Dirt:		ImpactFX = DirtFX; break;
		case LAYLA_SURFACE_Water:		ImpactFX = WaterFX; break;
		case LAYLA_SURFACE_Metal:		ImpactFX = MetalFX; break;
		case LAYLA_SURFACE_Wood:		ImpactFX = WoodFX; break;
		case LAYLA_SURFACE_Grass:		ImpactFX = GrassFX; break;
		case LAYLA_SURFACE_Glass:		ImpactFX = GlassFX; break;
		case LAYLA_SURFACE_Flesh:		ImpactFX = FleshFX; break;
		default:						ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* ALaylaImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = NULL;

	switch (SurfaceType)
	{
		case LAYLA_SURFACE_Concrete:	ImpactSound = ConcreteSound; break;
		case LAYLA_SURFACE_Dirt:		ImpactSound = DirtSound; break;
		case LAYLA_SURFACE_Water:		ImpactSound = WaterSound; break;
		case LAYLA_SURFACE_Metal:		ImpactSound = MetalSound; break;
		case LAYLA_SURFACE_Wood:		ImpactSound = WoodSound; break;
		case LAYLA_SURFACE_Grass:		ImpactSound = GrassSound; break;
		case LAYLA_SURFACE_Glass:		ImpactSound = GlassSound; break;
		case LAYLA_SURFACE_Flesh:		ImpactSound = FleshSound; break;
		default:						ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}
