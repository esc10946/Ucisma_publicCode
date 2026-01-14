// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/*
 * 
 */

class UCISMA_API IActorWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitWidget(AActor* Parent) = 0;

	virtual void UpdateValue() = 0;

	virtual void RemoveWidget() = 0;
};
