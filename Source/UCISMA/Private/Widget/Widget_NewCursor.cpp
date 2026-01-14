// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_NewCursor.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Manager/StageManager.h"

FTimerHandle CursorTimer;


void UWidget_NewCursor::NativeConstruct()
{
    Super::NativeConstruct();
    InitColor();
    InitPlayerData();
    InitData();
}

void UWidget_NewCursor::InitColor()
{
    decrease_color = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // R = 1, G = 0, B = 0, A = 1 (������)
    increase_color = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // R = 0, G = 1, B = 0, A = 1 (������)
    delete_color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f); // R = 0, G = 0, B = 0, A = 0 (���� ����)
    default_color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UWidget_NewCursor::InitPlayerData()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
    {
        return;
    }
    PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetCharacter());
    if (!PlayerCharacter)
    {
        return;
    }

    beatsecond = PlayerCharacter->beatpersecond;
    //PlayerCharacter->OnStatsChanged.AddDynamic(this, &UWiget_State::HandleStatsChanged);
    PlayerCharacter->OnHPChanged.AddDynamic(this, &UWidget_NewCursor::HandleStatsChanged);
    Update_Health(PlayerCharacter);
    Update_Dash(PlayerCharacter);
}

void UWidget_NewCursor::InitData()
{
    bIsHealFading = false;
    HealingCellIndex = -1;
    HealFadeAlpha = 1.1f;
}

void UWidget_NewCursor::NativeDestruct()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
    }

    Super::NativeDestruct();
}


void UWidget_NewCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (!PlayerCharacter)
    {
        return;
    }
    if (!bisfading) {
        Update_Health(PlayerCharacter);
    }
    FadeHeal(InDeltaTime);

    //대쉬
    Update_Dash(PlayerCharacter);
    if (bIsDashTrans && Dash_Bar)
    {
        ElapsedTime_Dash += InDeltaTime;
        float Alpha = FMath::Clamp(ElapsedTime_Dash / beatsecond, 0.0f, 1.0f);

        // 2단계 색상 정의
        FLinearColor Blue = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
        FLinearColor LightBlue = FLinearColor(0.5f, 0.75f, 1.0f, 1.0f);
        FLinearColor White = FLinearColor::White;

        FLinearColor CurrentColor;

        if (Alpha < 0.5f)
        {
            float LocalAlpha = Alpha / 0.5f; // 0~1 사이
            CurrentColor = FMath::Lerp(Blue, LightBlue, LocalAlpha);
        }
        else
        {
            float LocalAlpha = (Alpha - 0.5f) / 0.5f; // 0~1 사이
            CurrentColor = FMath::Lerp(LightBlue, White, LocalAlpha);
        }

        Dash_Bar->SetFillColorAndOpacity(CurrentColor);

        if (Alpha >= 1.0f)
        {
            bIsDashTrans = false;
        }
    }
}

void UWidget_NewCursor::FadeHeal(float TickDeltaTime)
{
    if (!bIsHealFading)
    {
        return;
    }

    UProgressBar* TargetWidget = nullptr;
    if (HealingCellIndex == 1)
    {
        TargetWidget = Health_Bar_3;
    }
    else if (HealingCellIndex == 2)
    {
        TargetWidget = Health_Bar_2;
    }
    else if (HealingCellIndex == 3)
    {
        TargetWidget = Health_Bar_1;
    }
    if (!TargetWidget)
    {
        LOG_DEBUG(Error, TEXT("New Cursor Can't Find HealingCellIndex"));
        return;
    }

    HealFadeAlpha += TickDeltaTime * PlayerCharacter->GetStageManager()->GetBeatPerSecond();
    FLinearColor NewColor = FMath::Lerp(increase_color, default_color, HealFadeAlpha);
    TargetWidget->SetPercent(1.0f);
    TargetWidget->SetFillColorAndOpacity(NewColor);

    if (HealFadeAlpha > 1.0f)
    {
        bIsHealFading = false;
    }

}

void UWidget_NewCursor::Event_Increase(int HP) {
    switch (HP) {
    case 1:
        Update_Health(PlayerCharacter);
        bIsHealFading = true;
        HealFadeAlpha = 0.0f;
        HealingCellIndex = 1;
        break;
    case 2:
        Update_Health(PlayerCharacter);
        bIsHealFading = true;
        HealFadeAlpha = 0.0f;
        HealingCellIndex = 2;
        break;
    case 3:
        Update_Health(PlayerCharacter);
        bIsHealFading = true;
        HealFadeAlpha = 0.0f;
        HealingCellIndex = 3;
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unexpected HP value in Event_Increase: %d"), HP);
        break;
    }
}
void UWidget_NewCursor::Event_Decrease(int HP) {
    switch (HP) {
    case 0:
        Update_Health(PlayerCharacter);
        bIsHealFading = false;
        HealFadeAlpha = 1.1f;
        break;
    case 1:
        Update_Health(PlayerCharacter);
        bIsHealFading = false;
        HealFadeAlpha = 1.1f;
        break;
    case 2:
        Update_Health(PlayerCharacter);
        bIsHealFading = false;
        HealFadeAlpha = 1.1f;
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unexpected HP value in Event_Decrease: %d"), HP);
        break;
    }


}

void UWidget_NewCursor::HandleStatsChanged(int Player_Current_HP, int changer)
{
    if (changer == 1)
    {
        Event_Increase(Player_Current_HP);
    }
    else
    {

        if (bisfading) {
            bisattacked = true;
        }

        Event_Decrease(Player_Current_HP);
    }
}

void UWidget_NewCursor::Update_Health(APlayerCharacter* InPlayerCharacter)
{
    //if (InPlayerCharacter && !Health_Bar_1 && !Health_Bar_2 && !Health_Bar_3)
    if (InPlayerCharacter)
    {
        int player_hp = InPlayerCharacter->Player_Current_HP;

        switch (player_hp) {
        case 0:
            Health_Bar_1->SetFillColorAndOpacity(delete_color);
            Health_Bar_2->SetFillColorAndOpacity(delete_color);
            Health_Bar_3->SetFillColorAndOpacity(increase_color);
            Health_Bar_3->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
            break;
        case 1:
            Health_Bar_1->SetFillColorAndOpacity(delete_color);
            Health_Bar_2->SetFillColorAndOpacity(increase_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);
            Health_Bar_2->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
            break;
        case 2:
            Health_Bar_1->SetFillColorAndOpacity(increase_color);
            Health_Bar_2->SetFillColorAndOpacity(default_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);
            Health_Bar_1->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
            break;
        case 3:
            Health_Bar_1->SetFillColorAndOpacity(default_color);
            Health_Bar_2->SetFillColorAndOpacity(default_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);

            Health_Bar_1->SetPercent(1.0f);
            Health_Bar_2->SetPercent(1.0f);
            Health_Bar_3->SetPercent(1.0f);

        }

    }
}

void UWidget_NewCursor::Update_Dash(APlayerCharacter* InPlayerCharacter)
{
    if (Dash_Bar && InPlayerCharacter)
    {
        float CurrentStamina = InPlayerCharacter->Player_Current_Stamina;
        float MaxStamina = InPlayerCharacter->Player_MAX_Stamina;

        Dash_Bar->SetPercent(CurrentStamina / MaxStamina);

        // 100 미만으로 떨어졌을 때 → 즉시 파란색
        if (LastStamina == 100.0f && CurrentStamina < 100.0f)
        {
            Dash_Bar->SetFillColorAndOpacity(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f));
            bIsDashTrans = false; // 이전 트랜지션 중지
        }

        // 다시 100이 되었을 때 → 부드럽게 흰색으로 전환
        if (CurrentStamina == 100.0f && LastStamina < 100.0f)
        {
            DashTargetColor = FLinearColor::White;
            ElapsedTime_Dash = 0.0f;
            bIsDashTrans = true;
        }

        LastStamina = CurrentStamina;
    }
}

//void UWidget_NewCursor::DefaultColor(UProgressBar* TargetImage) {
//    if (TargetImage)
//    {
//        TargetImage->SetFillColorAndOpacity(default_color);
//        UE_LOG(LogTemp, Warning, TEXT("Default"));
//    }
//    else {
//        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
//    }
//    Update_Health(PlayerCharacter);
//
//}


//void UWidget_NewCursor::Fade_HPCell(UProgressBar* HPCell, float Duration, bool binout) { //in = true | out = false;
//    if (!HPCell) return;
//
//    // 초기 색상 설정
//    FLinearColor StartColor = binout ? increase_color : default_color;
//    FLinearColor EndColor = binout ? default_color : delete_color;
//
//    // 대상 위젯 저장
//    UProgressBar* TargetWidget = HPCell;
//
//    if (!binout)
//    {
//        TargetWidget->SetFillColorAndOpacity(EndColor);
//        return;
//    }
//
//    TargetWidget->SetPercent(1.0f);
//
//    float LocalElapsedTime = 0.0f;
//    bisfading = true;
//
//    UWorld* World = GetWorld();
//    if (!World) return;
//
//    Update_Health(PlayerCharacter);
//
//    TWeakObjectPtr<UWidget_NewCursor> WeakThis(this);
//    FTimerHandle LocalFadeTimerHandle;
//
//    World->GetTimerManager().SetTimer(
//        LocalFadeTimerHandle,
//        [=]() mutable
//        {
//            if (!WeakThis.IsValid())
//            {
//                World->GetTimerManager().ClearTimer(LocalFadeTimerHandle);
//                return;
//            }
//
//            UWidget_NewCursor* SafeThis = WeakThis.Get();
//
//            // 공격 받았을 경우 즉시 종료
//            if (SafeThis->bisattacked)
//            {
//                SafeThis->bisfading = false;
//                if (SafeThis->PlayerCharacter)
//                {
//                    SafeThis->Update_Health(SafeThis->PlayerCharacter);
//                }
//                World->GetTimerManager().ClearTimer(LocalFadeTimerHandle);
//                return;
//            }
//
//            LocalElapsedTime += World->GetDeltaSeconds();
//            float Alpha = FMath::Clamp(LocalElapsedTime / Duration, 0.f, 1.f);
//
//            // RGB 보간
//            FLinearColor NewColor = FMath::Lerp(StartColor, EndColor, Alpha);
//            if (IsValid(TargetWidget))
//            {
//                TargetWidget->SetFillColorAndOpacity(NewColor);
//            }
//
//            if (Alpha >= 1.f)
//            {
//                SafeThis->bisfading = false;
//                if (SafeThis->PlayerCharacter)
//                {
//                    SafeThis->Update_Health(SafeThis->PlayerCharacter);
//                }
//                World->GetTimerManager().ClearTimer(LocalFadeTimerHandle);
//            }
//        },
//        0.01f,
//        true
//    );
//
//    // 외부에서 타이머 해제 가능하도록 저장
//    FadeTimerHandle = LocalFadeTimerHandle;
//}




//void UWidget_NewCursor::ResetColor(UProgressBar* TargetImage) {
//    if (TargetImage)
//    {
//        TargetImage->SetFillColorAndOpacity(delete_color);
//        UE_LOG(LogTemp, Warning, TEXT("ResetColor"));
//    }
//    Update_Health(PlayerCharacter);
//}



//void UWidget_NewCursor::Decrease_change_color(UProgressBar* TargetImage) {
//    if (TargetImage) {
//        //TargetImage->SetColorAndOpacity(decrease_color);
//   
//        //GetWorld()->GetTimerManager().SetTimer(CursorTimer, FTimerDelegate::CreateUObject(this, &UWidget_NewCursor::ResetColor, TargetImage), 0.5f, false);
//        UE_LOG(LogTemp, Warning, TEXT("decrease_change_color"));
//        TargetImage->SetFillColorAndOpacity(delete_color);
//        LOG_SGS_SCREEN(3.f, TEXT("감소실행"));
//    }
//    else {
//        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
//    }
//
//}
//
//void UWidget_NewCursor::Increase_change_color(UProgressBar* TargetImage) {
//    if (TargetImage) {
//        TargetImage->SetFillColorAndOpacity(default_color);
//        GetWorld()->GetTimerManager().SetTimer(CursorTimer, FTimerDelegate::CreateUObject(this, &UWidget_NewCursor::DefaultColor, TargetImage), beatsecond, false);
//
//        UE_LOG(LogTemp, Warning, TEXT("Increase_change_color"));
//    }
//    else {
//        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
//    }
//}



void UWidget_NewCursor::HP_Refresh(int Player_Current_HP) {

    /*
    
    switch (Player_Current_HP) {
        case 0:
            Health_Bar_1->SetFillColorAndOpacity(delete_color);
            Health_Bar_2->SetFillColorAndOpacity(delete_color);
            Health_Bar_3->SetFillColorAndOpacity(increase_color);
            Health_Bar_3->SetPercent(Player_Current_HP / InPlayerCharacter->Player_MAX_Health);
			break;
        case 1:
            Health_Bar_1->SetFillColorAndOpacity(delete_color);
            Health_Bar_2->SetFillColorAndOpacity(increase_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);
            Health_Bar_2->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
            break;
        case 2:
            Health_Bar_1->SetFillColorAndOpacity(increase_color);
            Health_Bar_2->SetFillColorAndOpacity(default_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);
            Health_Bar_1->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
            break;
        case 3:
            Health_Bar_1->SetFillColorAndOpacity(default_color);
            Health_Bar_2->SetFillColorAndOpacity(default_color);
            Health_Bar_3->SetFillColorAndOpacity(default_color);
    }

    */

    
}

