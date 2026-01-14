// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesign/Boss_Harmyth_DataAsset.h"

UBoss_Harmyth_DataAsset::UBoss_Harmyth_DataAsset()
{
    DefaultDamageInfos = { 
        {EBossBodyPart::Head, false},
        {EBossBodyPart::LeftHand, false},
        {EBossBodyPart::RightHand, false} 
    };
}
