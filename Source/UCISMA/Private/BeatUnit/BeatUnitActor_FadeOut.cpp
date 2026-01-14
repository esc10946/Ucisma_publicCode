// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor_FadeOut.h"
#include "Blueprint/UserWidget.h"
#include "Manager/BeatManager.h"
#include "Manager/StageManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include <Components/Image.h>

// Sets default values
ABeatUnitActor_FadeOut::ABeatUnitActor_FadeOut()
{
    PrimaryActorTick.bCanEverTick = true;

    TargetOpacity = 0.0f;
    BeatCount = 0;
    BeatDuration = 10;
    TimeLine = 0;
    FadeTiming = 4;
}

void ABeatUnitActor_FadeOut::BeginPlay()
{
    Super::BeginPlay();

    // Spawn and add widget to viewport

    //deWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), FadeWidgetClass);
    if (FadeWidget)
    {
        FadeWidget->AddToViewport();
    }

    SetActorHiddenInGame(false);
    bIsInitialized = false;
}

void ABeatUnitActor_FadeOut::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsInitialized) // 초기화 상태가 아니면 확인
    {
        AStageManager* stageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
        if (stageManager)
        {
            BeatDuration = stageManager->GetSecondPerBeat() * FadeTiming - 1;
            bIsInitialized = true; // 초기화 완료
        }
    }

    if (ABeatUnitActor::GetIsLiving() && BeatCount < FadeTiming)
    {
        TimeLine += DeltaTime;
        float alpha = FMath::Clamp((TimeLine / BeatDuration), 0.0f, 1.0f);

        UpdateWidgetOpacity(alpha);
    }
}

void ABeatUnitActor_FadeOut::BeatUpdate(int QuaterIndex)
{
    if (QuaterIndex == 0)
    {
        BeatCount++;
        if (BeatCount == FadeTiming)
        {
            UpdateWidgetOpacity(1.0f); // Fully opaque (white screen)

        }
        else if (BeatCount == FadeTiming + 1)
        {
            ChangeScene();
            UpdateWidgetOpacity(0.0f); // Fully transparent (normal screen)
        }
    }
}

// Update the widget's opacity dynamically
void ABeatUnitActor_FadeOut::UpdateWidgetOpacity(float alpha)
{
    UE_LOG(LogTemp, Warning, TEXT("alpha = %f"), alpha);
    if (FadeWidget)
    {
        UImage* WhiteImage = Cast<UImage>(FadeWidget->GetWidgetFromName(TEXT("WhiteFade")));
        if (WhiteImage)
        {
            // Adjust the widget's opacity using Slate Brush or material binding
            WhiteImage->SetRenderOpacity(alpha);
        }
    }
}