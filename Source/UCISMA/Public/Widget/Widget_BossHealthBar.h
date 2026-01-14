// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BossHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_BossHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

    UFUNCTION(BlueprintCallable)
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable)
	void DamageIndicator();

    // 배경 이미지 (체력 감소 시 색상 변화)
    UPROPERTY(meta = (BindWidget))
    class UImage* HealthBarBackground;

    // 색상 복귀 타이머
    FTimerHandle ColorResetTimer;

    UFUNCTION(BlueprintCallable)
    // 색상 복귀 메소드
    void ResetHealthBarColor();

    UFUNCTION(BlueprintCallable)
    void DestroyHealthBar();
    
    UFUNCTION(BlueprintImplementableEvent)
    void TriggerRemoveAnimation();
private:
    float timerHealthBar;
};
