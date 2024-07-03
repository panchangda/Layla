
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"
#include "IPickupable.generated.h"

/**  */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**  */
class IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void OnPick() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void OnDrop() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void OnFocused() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void OnFocusLost() = 0;
};