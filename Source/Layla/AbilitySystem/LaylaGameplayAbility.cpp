// Fill out your copyright notice in the Description page of Project Settings.


#include "LaylaGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Player/LaylaCharacter.h"

ULaylaGameplayAbility::ULaylaGameplayAbility()
{
	
}

void ULaylaGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

ALaylaCharacter* ULaylaGameplayAbility::GetLaylaCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ALaylaCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
