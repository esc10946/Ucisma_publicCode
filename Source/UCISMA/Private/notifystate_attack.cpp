// Fill out your copyright notice in the Description page of Project Settings.


#include "notifystate_attack.h"
#include "PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"


void Unotifystate_attack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    if (MeshComp)
    {
        AActor* Owner = MeshComp->GetOwner();
        if (Owner)
        {
            APlayerCharacter* Player = Cast<APlayerCharacter>(Owner);
            if (Player)
            {
                FLinearColor PlayerColor = Player->AttackColor;
                UFXSystemComponent* FXComponent = GetSpawnedEffect(MeshComp);
                if (FXComponent)
                {
                    static FName ColorParamName = "attack_color";
                    FXComponent->SetColorParameter(ColorParamName, PlayerColor);
                }
            }
        }
    }
}