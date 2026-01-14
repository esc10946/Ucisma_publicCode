// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "TimerManager.h" 
#include "Widget_Fadeout_HP.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API UWidget_Fadeout_HP : public UUserWidget
{
	GENERATED_BODY()
public:
	float TargetOpacity = 0.0f;    // 목표 Opacity 값
	float FadeDuration = 1.0f;     // 페이드 아웃에 걸리는 시간
	float StartOpacity = 1.0f;     // 시작 Opacity 값 
	float ElapsedTime = 0.0f; //경과 시간
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Image_plus;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Image_minus;

	UImage* TargetImage;
	FTimerHandle FadeTimerHandle;  // 타이머 핸들

	void FadeOut(float BPM, int pm);
	void UpdateOpacity();
};