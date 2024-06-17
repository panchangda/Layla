// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LaylaProjectile.generated.h"


UCLASS(BlueprintType, Blueprintable)
class LAYLA_API ALaylaProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaylaProjectile();

	// 球体碰撞组件。
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// 发射物移动组件。
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// 发射物网格体
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;

	// 发射物材质
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// 在投射物撞击其他对象并爆炸时使用的粒子。
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;
 
	//此投射物将造成的伤害类型和伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
 
	//此投射物造成的伤害。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float Damage;
	
	// 初始化射击方向上发射物速度的函数。
	void FireInDirection(const FVector& ShootDirection);

	// 当发射物击中物体时会调用的函数。
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void Destroyed() override;

	
	UFUNCTION(Category="Projectile")
	void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
