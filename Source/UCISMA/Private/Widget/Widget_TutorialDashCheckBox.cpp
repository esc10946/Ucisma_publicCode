// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_TutorialDashCheckBox.h"
#include "Components/Image.h"

void UWidget_TutorialDashCheckBox::Init(AStageManager* InputStageManager)
{
	UImage* PerfectDashBox = Cast<UImage>(GetWidgetFromName(TEXT("PerfectDashCheck_ON")));
	UImage* NormalDashBox = Cast<UImage>(GetWidgetFromName(TEXT("NormalDashCheck_ON")));
	CheckBoxes.Emplace(EJudgementResult::DashPerfect, PerfectDashBox);
	CheckBoxes.Emplace(EJudgementResult::DashSuccess, NormalDashBox);
	PerfectDashBox->SetVisibility(ESlateVisibility::Hidden);
	NormalDashBox->SetVisibility(ESlateVisibility::Hidden);

	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_TutorialDashCheckBox::ChangeDashCheckBox(EJudgementResult type)
{
	if (CheckBoxes.Contains(type)) {
		CheckBoxes[type]->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		LOG_KHJ(Error, TEXT("Wrong DashType Input"));
	}
}

