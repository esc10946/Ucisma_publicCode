// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_Fadeout_HP.h"
#include "Components/Image.h"
#include "Engine/World.h"

void UWidget_Fadeout_HP::FadeOut(float BPM, int pm)
{
    if (TargetImage)
    {
        TargetImage->SetVisibility(ESlateVisibility::Hidden);
    }
    //위젯 찾기
    if (pm == 1) {
        TargetImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_plus")));
        TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    }
    else {
        TargetImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_minus")));
        TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    }
    // BPM에 맞춰 페이드 지속 시간 계산
    FadeDuration = BPM;  // BPM에 맞게 지속 시간을 설정 (초 단위)

    if (TargetImage)
    {
        // 페이드 아웃을 시작
        StartOpacity = 1.0f;  // 시작 Opacity는 1 (완전 불투명)
        ElapsedTime = 0.0f;
        GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UWidget_Fadeout_HP::UpdateOpacity, 0.02f, true);  // 타이머 설정 (0.02초 간격으로 호출)
    }
}


void UWidget_Fadeout_HP::UpdateOpacity()
{
    if (!TargetImage) return;

    // 경과 시간에 따른 새로운 Opacity 값 계산
    float Alpha = FMath::Clamp(ElapsedTime / FadeDuration, 0.0f, 1.0f);
    float CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Alpha);

    // Opacity 값을 UI 이미지에 적용
    TargetImage->SetOpacity(CurrentOpacity);

    // 경과 시간 갱신
    ElapsedTime += 0.02f;

    //시간이 다 지났을 경우
    if (ElapsedTime >= FadeDuration)
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
        TargetImage->SetVisibility(ESlateVisibility::Hidden);
    }
}