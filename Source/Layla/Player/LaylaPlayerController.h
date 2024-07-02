// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LaylaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LAYLA_API ALaylaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ScoreBoardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* GameMenuAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MapAction;

	/** notify local client about deaths */
	void OnDeathMessage(class ALaylaPlayerState* KillerPlayerState, class ALaylaPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);
	

	/** respawn after dying */
	virtual void UnFreeze() override;

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
	virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSuicide();
	
	virtual void SetupInputComponent() override;

	
	class ALaylaHUD* GetLaylaHUD();
	
private:
	void ShowScoreBoard();
	void HideScoreBoard();
	
	void ToggleGameMenuVisibility();
	
};
