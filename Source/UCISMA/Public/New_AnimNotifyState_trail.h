// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Animation/AnimNotifies/AnimNotifyState_Trail.h"
#include "New_AnimNotifyState_trail.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UNew_AnimNotifyState_trail : public UAnimNotifyState_Trail
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Trail")
	void SetPSTemplate(UParticleSystem* NewPSTemplate);

};