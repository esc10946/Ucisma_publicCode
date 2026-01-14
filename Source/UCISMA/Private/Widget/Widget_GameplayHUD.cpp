// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_GameplayHUD.h"
#include "Manager/StageManager.h"
#include "Widget/Widget_SongProgressBar.h"
#include "Widget/Widget_Score.h"
#include "Widget/Widget_Combo.h"
#include "Widget/Widget_BeatBar.h"

void UWidget_GameplayHUD::Init(AStageManager* InputStageManager)
{
	BeatBar->Init(InputStageManager);
	SongProgressBar->Init(InputStageManager);
	Score->Init(InputStageManager);
	Combo->Init(InputStageManager);
}