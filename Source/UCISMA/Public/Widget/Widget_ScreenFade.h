// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ScreenFade.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_ScreenFade : public UUserWidget
{
	GENERATED_BODY()

public:

    // 애니메이션 변수 직접 바인딩 (이름은 WBP에서 지정한 애니메이션 이름과 일치)
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FadeAnimation;

    // 페이드 재생 함수
    UFUNCTION(BlueprintCallable)
    void PlayFade();
};
