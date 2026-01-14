// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Manager/UIManager.h"
#include "Widget_Purification.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API UWidget_Purification : public UUserWidget
{
	GENERATED_BODY()

public:
	//위젯 생성 후 가장 먼저 호출되는 함수 (생성자 제외)
	void Init(class AStageManager* InputStageManager);
	//카메라로부터 방향을 받으면 카메라의 해당 방향 끝에 남은 정화 횟수를 출력하는 함수
	void ShowPurificationFailureCount(FVector2D ActorScreenPos);
	//정화 횟수를 출력하는 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Highlight, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimPurificationHighlight;

	// 정화 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Purification", meta = (AllowPrivateAccess = "true"))
	int PurificationCount = 10;
private:
	//스테이지매니저
	class AStageManager* StageManager;
	//현재 플레이어의 스크린 사이즈
	FVector2D ScreenSize;
	//위젯의 텍스트
	class UTextBlock* PurificationCountText;

	bool CanGameOver = true;

};