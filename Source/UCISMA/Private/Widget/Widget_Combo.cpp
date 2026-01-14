// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_Combo.h"
#include "Components/TextBlock.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"

void UWidget_Combo::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
	ComboText = Cast<UTextBlock>(GetWidgetFromName("ComboText"));
}

void UWidget_Combo::UpdateCombo(int Value)
{
	if (ComboText)
	{
		ComboText->SetText(FText::AsNumber(Value));
	}
	if (ComboUpAnim)
	{
		PlayAnimation(ComboUpAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
	}
	
}

