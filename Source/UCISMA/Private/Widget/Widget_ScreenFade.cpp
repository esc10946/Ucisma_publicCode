// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_ScreenFade.h"

void UWidget_ScreenFade::PlayFade()
{
    if (FadeAnimation)
    {
        //UE_LOG(LogTemp, Warning, TEXT("?? FadeAnimation is valid, playing!"));
        PlayAnimation(FadeAnimation);
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("? FadeAnimation is NULL! 애니메이션 연결 안 됨!"));
    }
}
