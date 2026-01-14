// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_TutorialText.h"
#include "Widget/Widget_WASDKeyBoard.h"
#include "Widget/Widget_TutorialAttackCount.h"
#include "Widget/Widget_TutorialDashCheckBox.h"
#include "Manager/TutorialManager.h"
#include "Manager/StageManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWidget_TutorialText::Init(AStageManager* InputStageManager)
{
	stageManager = InputStageManager;
	TutorialManagerComp = stageManager->GetTutorialManager();
	TutorialManagerComp->OnTutorialDashPressed.AddDynamic(this, &UWidget_TutorialText::ChangeDashCheckBox);
	TutorialManagerComp->OnTutorialAttackPressed.AddDynamic(this, &UWidget_TutorialText::ChangeAttackCheckBox);
	TutorialManagerComp->OnTutorialStepChanged.AddDynamic(this, &UWidget_TutorialText::ChangeStep);
	TutorialManagerComp->OnTutorialWarningAnimation.AddDynamic(this, &UWidget_TutorialText::PlayanimationWarning);

	InitArrowImages();
	mainScript = Cast<UTextBlock>(GetWidgetFromName(TEXT("MainScriptBlock")));
	subScript = Cast<UTextBlock>(GetWidgetFromName(TEXT("SubScriptBlock")));
	WidgetWASD = Cast<UWidget_WASDKeyBoard>(GetWidgetFromName(TEXT("WB_WASD")));
	if (WidgetWASD) {
		WidgetWASD->Init();
	}
	WidgetDashCountBox = Cast<UWidget_TutorialDashCheckBox>(GetWidgetFromName(TEXT("WB_TutorialDashCheckBox")));
	if (WidgetDashCountBox) {
		WidgetDashCountBox->Init(InputStageManager);
		//WidgetDashCountBox->ChangeDashCheckBox(EJudgementResult::DashSuccess);
	}
	WidgetAttackCountBox = Cast<UWidget_TutorialAttackCount>(GetWidgetFromName(TEXT("WB_TutorialAttackCount")));
	if (WidgetAttackCountBox) {
		WidgetAttackCountBox->Init(8);
	}
	WidgetAttackCountBoxStep8 = Cast<UWidget_TutorialAttackCount>(GetWidgetFromName(TEXT("WB_TutorialAttackCountStep8")));
	if (WidgetAttackCountBoxStep8) {
		WidgetAttackCountBoxStep8->Init(4);
	}

	TutorialStepWidgets.Emplace(ETutorialStep::Step4, WidgetWASD);
	TutorialStepWidgets.Emplace(ETutorialStep::Step6, WidgetDashCountBox);
	TutorialStepWidgets.Emplace(ETutorialStep::Step7, WidgetAttackCountBox);
	TutorialStepWidgets.Emplace(ETutorialStep::Step8, WidgetAttackCountBoxStep8);
}

void UWidget_TutorialText::OutputTutorialText(const FText& mainText, const FText& subText)
{
	if (GetVisibility() == ESlateVisibility::Hidden) SetVisibility(ESlateVisibility::Visible);

	if (mainScript && subScript) {
		mainScript->SetText(mainText);
		subScript->SetText(subText);
	}
}

void UWidget_TutorialText::OnPressedKeyRight(ETutorialKeyType PressedKey)
{
	WidgetWASD->ChangeKeyBoardImage(PressedKey);
}

void UWidget_TutorialText::InitArrowImages()
{
	songProcessBarArrow = Cast<UImage>(GetWidgetFromName(TEXT("SongProcessBarArrow")));
	attackNodeArrow = Cast<UImage>(GetWidgetFromName(TEXT("AttackNodeArrow")));
	dashNodeArrow = Cast<UImage>(GetWidgetFromName(TEXT("DashNodeArrow")));
	beatBarArrow = Cast<UImage>(GetWidgetFromName(TEXT("BeatBarArrow")));
	cursorArrow = Cast<UImage>(GetWidgetFromName(TEXT("CursorArrow")));

	ArrowImages.FindOrAdd(ETutorialStep::Step2).Add(songProcessBarArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step3).Add(cursorArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step6).Add(beatBarArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step6).Add(dashNodeArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step7).Add(beatBarArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step7).Add(attackNodeArrow);
	ArrowImages.FindOrAdd(ETutorialStep::Step9).Add(cursorArrow);

	for (uint8 i = 0; i <= static_cast<uint8>(ETutorialStep::StepFinal); ++i)
	{
		ETutorialStep Key = static_cast<ETutorialStep>(i);
		if (!ArrowImages.Contains(Key)) continue;

		for (auto& image : ArrowImages[Key]) {
			image->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	ChaseCusorStart();
}

void UWidget_TutorialText::VisibleArrowImage(ETutorialStep type)
{
	HiddenArrowImage();
	for (auto& Elem : ArrowImages)
	{
		ETutorialStep Key = Elem.Key;
		TArray<UImage*>& Images = Elem.Value;

		//LOG_KHJ(Warning, TEXT("input type = %d, other type = %d, It is %s"), static_cast<uint8>(type) , static_cast<uint8>(Key), (Key == type) ? TEXT("True") : TEXT("False"));

		if (Key == type) {
			for (UImage* image : Images)
			{
				if (image)
				{
					image->SetVisibility(ESlateVisibility::Visible);
					// ÀÌ¹ÌÁö ÀÌ¸§±îÁö Ãâ·Â
					//LOG_KHJ(Warning, TEXT(" - Image set to %s"), *image->GetName());
				}
				else
				{
					LOG_KHJ(Error, TEXT(" - Image pointer is nullptr!"));
				}
			}
		}
	}
}

void UWidget_TutorialText::HiddenArrowImage()
{
	songProcessBarArrow->SetVisibility(ESlateVisibility::Hidden);
	attackNodeArrow->SetVisibility(ESlateVisibility::Hidden);
	dashNodeArrow->SetVisibility(ESlateVisibility::Hidden);
	beatBarArrow->SetVisibility(ESlateVisibility::Hidden);
	cursorArrow->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_TutorialText::ChangeDashCheckBox(EJudgementResult type)
{
	WidgetDashCountBox->ChangeDashCheckBox(type);
}

void UWidget_TutorialText::ChangeAttackCheckBox(int32 count, int32 attackCount)
{
	WidgetAttackCountBox->ChangeAttackCheckBox(count);
	WidgetAttackCountBoxStep8->ChangeAttackCheckBox(attackCount);
}

void UWidget_TutorialText::ChangeStep(ETutorialStep Step)
{
	VisibleArrowImage(Step);

	// ¸ðµç À§Á¬ ¼û±è
	for (auto& Elem : TutorialStepWidgets)
	{
		if (Elem.Value)
		{
			Elem.Value->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// ÇØ´ç ½ºÅÜ À§Á¬¸¸ Ç¥½Ã
	if (TutorialStepWidgets.Contains(Step) && TutorialStepWidgets[Step])
	{
		TutorialStepWidgets[Step]->SetVisibility(ESlateVisibility::Visible);
	}
}
