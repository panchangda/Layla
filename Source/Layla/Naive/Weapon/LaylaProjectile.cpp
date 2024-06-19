// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// Sets default values
ALaylaProjectile::ALaylaProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if(!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if(!CollisionComponent)
	{
		// 用球体进行简单的碰撞展示。
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		
		// 将球体的碰撞配置文件名称设置为"Projectile"。
		// CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		// 组件击中某物时调用的事件。
		// CollisionComponent->OnComponentHit.AddDynamic(this, &ALaylaProjectile::OnHit);
		//在击中事件上注册此投射物撞击函数。

		CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		if (GetLocalRole() == ROLE_Authority)
		{
			CollisionComponent->OnComponentHit.AddDynamic(this, &ALaylaProjectile::OnProjectileImpact);
		}
		// 设置球体的碰撞半径。
		CollisionComponent->InitSphereRadius(10.0f);
		// 将根组件设置为碰撞组件。
		RootComponent = CollisionComponent;
	}

	if(!ProjectileMovementComponent)
	{
		// 使用此组件驱动发射物的移动。
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 1500.0f;
		ProjectileMovementComponent->MaxSpeed = 1500.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if(!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Projectile/SM_Sphere.SM_Sphere'"));
		if(Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/Projectile/M_DefaultProjectile.M_DefaultProjectile'"));
		if (Material.Succeeded())
		{
			ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
		}
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
		ProjectileMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -5.0f));
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);
	}

	// Particle Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("'/Game/Projectile/P_Explosion.P_Explosion'"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}
	
	
	// 5 秒后删除发射物。
	InitialLifeSpan = 5.0f;


	// Network
	bReplicates = true;

	// Damage Class & Value
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
	
}

void ALaylaProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void ALaylaProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
	Destroy();
}


// Called when the game starts or when spawned
void ALaylaProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALaylaProjectile::Destroyed()
{
	Super::Destroyed();
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}

void ALaylaProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if ( OtherActor )
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
	}

	Destroy();
}

// Called every frame
void ALaylaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

