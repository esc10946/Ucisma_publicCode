// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_AdjustJudgement.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"
#include "Components/TextBlock.h"

void UWidget_AdjustJudgement::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
	BeatIndex = 0;
	StageManager->BeatUpdateListeners.AddDynamic(this, &UWidget_AdjustJudgement::BeatUpdate);
}

void UWidget_AdjustJudgement::BeatUpdate(int QuaterIndex)
{
	/*FString DebugMessage = 
	(TEXT("Widget_AdjustJudgement > BeatUpdate > Execute"));
	GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::White, DebugMessage);
	if (QuaterIndex == 0)
	{
		BeatIndex++;
		if (BeatRectangleAnim && BeatIndex <= 16)
		{
			PlayRectangleAnim();
		}
	}*/
}

void UWidget_AdjustJudgement::PlayRectangleAnim()
{
	PlayAnimation(BeatRectangleAnim, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
}
