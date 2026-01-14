// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_Purification.h"
#include "Engine/Engine.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"

#define SCREEN_PERCENTAGE_FOR_EDGE_GAP 0.95

void UWidget_Purification::Init(AStageManager* InputStageManager)
{
	//변수 초기화
	StageManager = InputStageManager;
	GEngine->GameViewport->GetViewportSize(ScreenSize);
	PurificationCountText = Cast<UTextBlock>(GetWidgetFromName("PurificationCount"));
	if (!PurificationCountText)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget_Purification > Init > Can't cast PurificationCount"));
	}
	CanGameOver = true;
}

void UWidget_Purification::ShowPurificationFailureCount(FVector2D ActorScreenPos)
{
	float ScreenWidth = ScreenSize.X;
	float ScreenHeight = ScreenSize.Y;
	//화면 밖에 출력되는 것을 방지하기 위해 화면 크기의 0.95 범위 안에 출력되도록 바꿈
	ActorScreenPos =
	{
		FMath::Clamp(ActorScreenPos.X,
			ScreenWidth * (1 - SCREEN_PERCENTAGE_FOR_EDGE_GAP),
			ScreenWidth * SCREEN_PERCENTAGE_FOR_EDGE_GAP),
		FMath::Clamp(ActorScreenPos.Y,
			ScreenHeight * (1 - SCREEN_PERCENTAGE_FOR_EDGE_GAP),
			ScreenHeight * SCREEN_PERCENTAGE_FOR_EDGE_GAP)
	};


	PurificationCountText->SetText(FText::AsNumber(StageManager->GetMaxPurificationFailureCount() - StageManager->GetPurificationFailureCount()));
	

	UCanvasPanelSlot* Canvas = Cast<UCanvasPanelSlot>(PurificationCountText->Slot);
	if (Canvas)
	{
		Canvas->SetPosition(ActorScreenPos);
		PlayAnimation(AnimPurificationHighlight, 0.0f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
	}
	else
	{
		FString DebugMessage = FString::Printf(TEXT("[Error] Camera_Map > OnOverlapEnd > Purification Fail > Cast UCanvasPanelSlot Failed"));
		GEngine->AddOnScreenDebugMessage(30, 5.0, FColor::Red, DebugMessage);
	}
}