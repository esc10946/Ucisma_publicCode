// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Score.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API UWidget_Score : public UUserWidget
{
	GENERATED_BODY()

public:
	//위젯이 생성된 후 가장 먼저 호출되는 함수
	void Init(class AStageManager* InputStageManager);
	//Score로 점수 출력을 최신화하는 함수
	void UpdateScore(int Score);
	//점수 추가 애니메이션 실행
	UFUNCTION()
	void PlayScoreIncreaseAnim(int IncreaseAmount);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AddScore;
private:
	//점수를 출력할 텍스트
	class UTextBlock* ScoreText;
	//추가 점수를 출력할 텍스트
	class UTextBlock* ScoreIncreaseText;
	//스테이지 매니저
	class AStageManager* StageManager;

};