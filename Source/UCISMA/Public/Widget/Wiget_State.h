// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h" 
#include "../PlayerCharacter.h"
#include "Wiget_State.generated.h"

class APlayerCharacter;
/**
 *  플레이어 스탯
 *	PlayerCharacter 헤더파일 변수
	float Player_Current_HP;
	float Player_MAX_HP = 100.0f;
	float Player_Current_Stamina;
	float Player_MAX_Stamina = 100.0f;

 */
UCLASS()
class UCISMA_API UWiget_State : public UUserWidget
{
	GENERATED_BODY()
public:
	// 대시 바 (스태미나)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Dash_Bar;

	// 체력 바
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Health_Bar;

	UFUNCTION()
	void Init(class AStageManager* InputStageManager);
protected:
	// 초기화 함수 (UI에 바인딩 및 값 업데이트)
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void Update_Health(APlayerCharacter* PlayerCharacter);
	void Update_Dash(APlayerCharacter* PlayerCharacter);

private:
	// 플레이어 캐릭터 참조
	APlayerCharacter* PlayerCharacter;
	class AStageManager* StageManager;

	// 체력과 스태미나를 업데이트하는 함수들


	// 스탯이 변경될 때 호출되는 델리게이트 처리 함수
	//UFUNCTION()
	//void HandleStatsChanged(float CurrentHealthPercent, float CurrentStaminaPercent);

};