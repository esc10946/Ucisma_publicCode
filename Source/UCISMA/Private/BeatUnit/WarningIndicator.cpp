// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/WarningIndicator.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/Image.h"

AWarningIndicator::AWarningIndicator()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 위젯 컴포넌트 생성 (경고 이미지)
    WarningWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WarningWidget"));
    RootComponent = WarningWidget;

    WarningWidget->SetCastShadow(false); 
    //static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Enemy/Obstacles/Warning/WBP_WarningIndicator.WBP_WarningIndicator_C"));
}

void AWarningIndicator::BeginPlay()
{
    Super::BeginPlay();
    
    if (WarningWidgetClass)
    {
        WarningWidget->SetWidgetClass(WarningWidgetClass);
    }

    WarningWidget->SetVisibility(true);
}

void AWarningIndicator::BeatUpdate(int QuaterIndex)
{
    Super::BeatUpdate(QuaterIndex);

    if (QuaterIndex) return;
    CurrentBeat++;
    UpdateWarningImage();
}

void AWarningIndicator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetIsLiving()) return;

    // 🔹 카메라 따라가기
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        FVector CamLocation;
        FRotator CamRotation;

    }
}

void AWarningIndicator::DeactiveUnit()
{
    WarningWidget->SetVisibility(false);
    Super::DeactiveUnit();
}

// =================== [이미지 깜빡이기] ===================
void AWarningIndicator::UpdateWarningImage()
{
    UUserWidget* Widget = WarningWidget->GetUserWidgetObject();
    if (Widget)
    {
        UImage* WarningImageA = Cast<UImage>(Widget->GetWidgetFromName(TEXT("WarningImageA")));
        UImage* WarningImageB = Cast<UImage>(Widget->GetWidgetFromName(TEXT("WarningImageB")));
        UImage* WarningImageC = Cast<UImage>(Widget->GetWidgetFromName(TEXT("WarningImageC")));

        if (WarningImageA && WarningImageB && WarningImageC)
        {
            int32 BeatPhase = CurrentBeat % 3;

            WarningImageA->SetVisibility(BeatPhase == 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            WarningImageB->SetVisibility(BeatPhase == 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            WarningImageC->SetVisibility(BeatPhase == 2 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }
    }
}
