// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_Cursor.h"
#include "PlayerCharacter.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"


FTimerHandle TimerHandle;

void UWidget_Cursor::NativeConstruct()
{
    Super::NativeConstruct();

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWidget_Cursor::FindPlayerCharacter, 0.5f, false);


}

void UWidget_Cursor::FindPlayerCharacter()
{
    // GetOwningPlayer() 대신 GetFirstPlayerController() 사용
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        // GetCharacter()를 통해 PlayerCharacter를 가져온 후 PlayerCharacterInstance에 저장
        PlayerCharacterInstance = Cast<APlayerCharacter>(PlayerController->GetCharacter());

        if (PlayerCharacterInstance)
        {
            // PlayerCharacterInstance로 캐릭터에 접근
            PlayerCharacterInstance->OnHPChanged.AddDynamic(this, &UWidget_Cursor::HandleStatsChanged);
            LOG_SGS_SCREEN(3.f, TEXT("hpbar 캐릭터에 접근"));
        }
        else
        {
            LOG_SGS_SCREEN(3.f, TEXT("hpbar 캐릭터에 접근실패"));
        }
    }
    else
    {
        LOG_SGS_SCREEN(3.f, TEXT("컨트롤러 없음"));
    }
}


void UWidget_Cursor::Init()
{
    // 빨강 (Red)
    decrease_color = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // R = 1, G = 0, B = 0, A = 1 (불투명)

    // 초록 (Green)
    increase_color = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // R = 0, G = 1, B = 0, A = 1 (불투명)

    // 투명 (Transparent)
    delete_color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f); // R = 0, G = 0, B = 0, A = 0 (완전 투명)

    default_color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}



void UWidget_Cursor::ResetColor(UImage* TargetImage) {
    if (TargetImage)
    {
        // 색상을 delete_color로 변경
        TargetImage->SetColorAndOpacity(delete_color);
        UE_LOG(LogTemp, Warning, TEXT("ResetColor"));
    }
    HP_Refresh(PlayerCharacterInstance->Player_Current_HP);


}

void UWidget_Cursor::DefaultColor(UImage* TargetImage) {
    if (TargetImage)
    {
        // 색상을 delete_color로 변경
        TargetImage->SetColorAndOpacity(default_color);
        UE_LOG(LogTemp, Warning, TEXT("Default"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
    }
    HP_Refresh(PlayerCharacterInstance->Player_Current_HP);

}

void UWidget_Cursor::Decrease_change_color(UImage* TargetImage) {
    if (TargetImage) {
        // 색상을 decrease_color로 변경
        TargetImage->SetColorAndOpacity(decrease_color);
        // 0.5초 딜레이 후 색상을 delete_color로 변경
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UWidget_Cursor::ResetColor, TargetImage), 0.5f, false);
        UE_LOG(LogTemp, Warning, TEXT("decrease_change_color"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
    }

}


void UWidget_Cursor::Increase_change_color(UImage* TargetImage) {
    if (TargetImage) {
        // 색상을 decrease_color로 변경
        TargetImage->SetColorAndOpacity(increase_color);
        // 0.5초 딜레이 후 색상을 default color로 변경
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UWidget_Cursor::DefaultColor, TargetImage), 0.5f, false);

        UE_LOG(LogTemp, Warning, TEXT("Increase_change_color"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("error_TargetImage"));
    }


}

void UWidget_Cursor::Event_Increase(int HP) {
    switch (HP) {
    case 1:
        Increase_change_color(HPCell_3);
        break;
    case 2:
        Increase_change_color(HPCell_2);
        break;
    case 3:
        Increase_change_color(HPCell_1);
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unexpected HP value in Event_Increase: %d"), HP);
        break;
    }
}
void UWidget_Cursor::Event_Decrease(int HP) {
    switch (HP) {
    case 0:
        Decrease_change_color(HPCell_3);
        break;
    case 1:
        Decrease_change_color(HPCell_2);
        break;
    case 2:
        Decrease_change_color(HPCell_1);
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unexpected HP value in Event_Decrease: %d"), HP);
        break;
    }


}


void UWidget_Cursor::HandleStatsChanged(int Player_Current_HP, int changer)
{
    if (changer == 1)
    {
        Event_Increase(Player_Current_HP);
    }
    else
    {
        Event_Decrease(Player_Current_HP);
    }
}

void UWidget_Cursor::HP_Refresh(int Player_Current_HP) {
    switch (Player_Current_HP) {
    case 0:
        HPCell_1->SetColorAndOpacity(delete_color);
        HPCell_2->SetColorAndOpacity(delete_color);
        HPCell_3->SetColorAndOpacity(delete_color);
        break;
    case 1:
        HPCell_1->SetColorAndOpacity(delete_color);
        HPCell_2->SetColorAndOpacity(delete_color);
        HPCell_3->SetColorAndOpacity(default_color);
        break;
    case 2:
        HPCell_1->SetColorAndOpacity(delete_color);
        HPCell_2->SetColorAndOpacity(default_color);
        HPCell_3->SetColorAndOpacity(default_color);
        break;
    case 3:
        HPCell_1->SetColorAndOpacity(default_color);
        HPCell_2->SetColorAndOpacity(default_color);
        HPCell_3->SetColorAndOpacity(default_color);
        break;
    }
}