// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "UObject/Interface.h"
#include "BeatUnitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBeatUnitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class UCISMA_API IBeatUnitInterface
{
	GENERATED_BODY()
public:
	//비트 매니저로부터 1/4비트마다 호출되는 함수
	UFUNCTION()
	virtual void BeatUpdate(int QuaterIndex) = 0;
	//유닛 비활성화 함수
	virtual void DeactiveUnit() = 0;
	//유닛 활성화 함수, 비트 매니저에 의해 소환될 때 호출된다.
	virtual void ActiveUnit() = 0;

	/*** Get함수들 ***/
	virtual const int GetBeatIndex() const = 0;
	virtual const int GetQuaterBeatIndex() const = 0;
	virtual bool GetIsLiving() = 0;

	//virtual void TakeDamage(float DamageAmount) {};
	virtual void OnEnemyHitFx(int index) {};
 
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
};
