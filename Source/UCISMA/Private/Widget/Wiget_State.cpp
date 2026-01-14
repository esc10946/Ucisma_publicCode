#include "Widget/Wiget_State.h"
#include "Manager/StageManager.h"
#include "PlayerCharacter.h"

void UWiget_State::Init(AStageManager* InputStageManager)
{
    StageManager = InputStageManager;
}

void UWiget_State::NativeConstruct()
{
    Super::NativeConstruct();

    // 플레이어 캐릭터 참조
    PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());  // 클래스 멤버에 할당
    if (PlayerCharacter)
    {
        // 델리게이트에 이벤트 바인딩
        //PlayerCharacter->OnStatsChanged.AddDynamic(this, &UWiget_State::HandleStatsChanged);

        // 초기 값 업데이트
        Update_Health(PlayerCharacter);
        Update_Dash(PlayerCharacter);
    }
}


void UWiget_State::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    // PlayerCharacter가 유효할 때만 업데이트
    if (PlayerCharacter)
    {
        Update_Health(PlayerCharacter);
        Update_Dash(PlayerCharacter);
    }
}

void UWiget_State::Update_Health(APlayerCharacter* InPlayerCharacter)
{
    if (Health_Bar && InPlayerCharacter)
    {
        // 플레이어의 현재 체력 비율로 프로그래스 바 업데이트
        Health_Bar->SetPercent(InPlayerCharacter->Player_Current_Health / InPlayerCharacter->Player_MAX_Health);
    }
}

void UWiget_State::Update_Dash(APlayerCharacter* InPlayerCharacter)
{
    if (Dash_Bar && InPlayerCharacter)
    {
        // 플레이어의 현재 스태미나 비율로 프로그래스 바 업데이트
        Dash_Bar->SetPercent(InPlayerCharacter->Player_Current_Stamina / InPlayerCharacter->Player_MAX_Stamina);
    }
}