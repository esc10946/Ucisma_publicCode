// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_BossHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"



void UWidget_BossHealthBar::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (!HealthBar) return;

    float HealthPercent = CurrentHealth / MaxHealth;
    HealthBar->SetPercent(HealthPercent);
}

void UWidget_BossHealthBar::DamageIndicator() 
{
    if (!HealthBarBackground || !HealthBar) return;
    GetWorld()->GetTimerManager().ClearTimer(ColorResetTimer);
    timerHealthBar = 0;

    // 배경과 체력바를 빨간색으로 변경
    HealthBarBackground->SetColorAndOpacity(FLinearColor::Red);
    HealthBar->SetFillColorAndOpacity(FLinearColor::Red);

    // 0.5초 후 원래 색상으로 복귀
    GetWorld()->GetTimerManager().SetTimer(ColorResetTimer, this, &UWidget_BossHealthBar::ResetHealthBarColor, 0.01f, true);
}

void UWidget_BossHealthBar::ResetHealthBarColor()
{
    // 서서히 원래 색으로 복귀 (흰색으로)
    //FLinearColor OriginalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //HealthBarBackground->SetColorAndOpacity(OriginalColor);
    //HealthBar->SetFillColorAndOpacity(OriginalColor);

    timerHealthBar += 0.01;

    FLinearColor ChangeColor = FMath::Lerp(FLinearColor::Red, FLinearColor::White, timerHealthBar / 0.5);

    HealthBarBackground->SetColorAndOpacity(ChangeColor);
    HealthBar->SetFillColorAndOpacity(ChangeColor);

    if (timerHealthBar >= 0.5) {
        GetWorld()->GetTimerManager().ClearTimer(ColorResetTimer);
        timerHealthBar = 0;

        HealthBarBackground->SetColorAndOpacity(FLinearColor::White);
        HealthBar->SetFillColorAndOpacity(FLinearColor::White);
    }
    
    //DestroyHealthBar();
}

void UWidget_BossHealthBar::DestroyHealthBar()
{
    TriggerRemoveAnimation();
    //RemoveFromParent();
}
