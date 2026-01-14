// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "notifystate_attack.generated.h"

/**
 * 
 */


class UNiagaraComponent;
class APlayerCharacter;


UCLASS()
class UCISMA_API Unotifystate_attack : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

};
