// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SongProgressBar.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API UWidget_SongProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	//위젯 생성 후 가장 먼저 호출되는 함수 (생성자 제외)
	void Init(class AStageManager* InputStageManager);
	//1/4비트마다 호출되는 함수, 델리게이트에 선헌함
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);

private:
	class AStageManager* StageManager;
	//출력되는 프로그래스바
	class UProgressBar* SongProgressBar;

};