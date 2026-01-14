// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_SongProgressBar.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"
#include "Components/ProgressBar.h"

void UWidget_SongProgressBar::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
	StageManager->BeatUpdateListeners.AddDynamic(this, &UWidget_SongProgressBar::BeatUpdate);
	SongProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("SongProgressBar")));
}

void UWidget_SongProgressBar::BeatUpdate(int QuaterIndex)
{
	SongProgressBar->SetPercent(static_cast<float>(StageManager->GetBeatIndex()) / StageManager->GetLastBeatIndex());
}