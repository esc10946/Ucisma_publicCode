// Fill out your copyright notice in the Description page of Project Settings.


#include "New_AnimNotifyState_trail.h"

void UNew_AnimNotifyState_trail::SetPSTemplate(UParticleSystem* NewPSTemplate)
{
    if (NewPSTemplate != nullptr)
    {
        PSTemplate = NewPSTemplate;
    }
}