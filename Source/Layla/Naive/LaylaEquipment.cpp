// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaEquipment.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


ULaylaEquipment::ULaylaEquipment(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

APawn* ULaylaEquipment::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* ULaylaEquipment::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void ULaylaEquipment::OnEquipped()
{
	
}

void ULaylaEquipment::OnUnequipped()
{
	
}

// For Network Rep
void ULaylaEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, SpawnedActors);
	
}
void ULaylaEquipment::SpawnEquipmentActors(const TArray<FLaylaEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FLaylaEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void ULaylaEquipment::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}