// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_Score.h"
#include "Components/TextBlock.h"
#include "Manager/StageManager.h"

void UWidget_Score::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;

	ScoreText = Cast<UTextBlock>(GetWidgetFromName("ScoreText"));
	ScoreIncreaseText = Cast<UTextBlock>(GetWidgetFromName("ScoreIncreaseText"));
}

void UWidget_Score::UpdateScore(int Score)
{
	ScoreText->SetText(FText::AsNumber(Score));
}

void UWidget_Score::PlayScoreIncreaseAnim(int IncreaseAmount)
{
	ScoreIncreaseText->SetText(FText::AsNumber(IncreaseAmount));
	PlayAnimation(AddScore, 0.0f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
}

