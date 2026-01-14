// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_DebugMode.h"
#include "Manager/StageManager.h"

void UWidget_DebugMode::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
}