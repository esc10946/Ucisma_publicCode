// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "UObject/Interface.h"
#include "System/CommonTypes.h"
#include "ProjectileInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UCISMA_API IProjectileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Init함수, 이 함수가 호출되어야 투사체가 발사된다.
	UFUNCTION()
	virtual void Shoot(FProjectileInformation ProjectileInfo) = 0;
protected:
	UFUNCTION()
	virtual void DestroyProjectile() = 0;
private:
};
