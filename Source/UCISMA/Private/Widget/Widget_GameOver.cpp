// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_GameOver.h"
#include "Manager/StageManager.h"
#include "Components/Image.h"

void UWidget_GameOver::NativeConstruct()
{
	Super::NativeConstruct();
	for (int32 i = 0; i < StaticEnum<EGameOverType>()->NumEnums() - 1; i++)
	{
		if (!GameOverTextures[static_cast<EGameOverType>(i)])
		{
			LOG_DEBUG(Warning, TEXT("Check Widget_GameOver>GameOverTextures"));
		}
	}
}

void UWidget_GameOver::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
}