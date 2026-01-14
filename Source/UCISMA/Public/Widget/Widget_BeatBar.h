// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Manager/StageManager.h"
#include "Widget_BeatBar.generated.h"
#define NOTE_SLIDER_COUNT 2

UENUM(BlueprintType)
enum class EJudgement : uint8
{
	AttackPerfect UMETA(DisplayName = "Attack_Perfect"),
	AttackGreat UMETA(DisplayName = "Attack_Great"),
	AttackGood UMETA(DisplayName = "Attack_Good"),
	AttackNormal UMETA(DisplayName = "Attack_Normal"),
	DashSuccess UMETA(DisplayName = "Dash_Success"),
	DashFail UMETA(DisplayName = "Dash_Fail")
};
/**
 *
 */
UCLASS()
class UCISMA_API UWidget_BeatBar : public UUserWidget
{
	GENERATED_BODY()

public:
	//초기화
	void Init(class AStageManager* InputStageManager);
	//BeatManager의 BeatUpdate에 의해 1/4박자마다 호출되는 함수, 델리게이트에 선언함
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);
	//비트바에 있는 두 노트를 삭제하는 함수
	void DeleteNote(EJudgementAction Action);
	void ShowJudgementHighlight(EJudgementResult Judgement);

	//공격 판정 시 비트바에 뜨는 하이라이트 애니메이션을 할당한 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Highlight, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimAttackHighlight;
	//대쉬 판정 시 비트바에 뜨는 하이라이트 애니메이션을 할당한 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Highlight, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimDashHighlight;

private:
	//UI에서 사용하는 Tick 함수
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//노트가 가운데로 움직이는 등 노트의 상태를 갱신하는 함수
	void UpdateNote(float DeltaTime);

	//노트
	class USlider* AttackNotes[NOTE_SLIDER_COUNT];
	class USlider* DashNotes[NOTE_SLIDER_COUNT];
	//노트가 판정으로부터 얼마나 멀어져있는지를 실수로 나타낸 변수
	float AttackNoteValues[NOTE_SLIDER_COUNT];
	float DashNoteValues[NOTE_SLIDER_COUNT];
	//비트바에 공격 노트를 생성하는 함수
	void SpawnAttackNote();
	//비트바에 대쉬 노트를 생성하는 함수
	void SpawnDashNote();
	//StageManager로부터 가져온 Bpm
	int Bpm;
	//공격 하이라이트
	class UImage* AttackJudgementHighlight;
	//대쉬 하이라이트
	class UImage* DashJudgementHighlight;
	//각 판정에 따른 하이라이트 텍스쳐를 블루프린트에서 받아오는 변수
	UPROPERTY(EditAnywhere, Category = Highlight)
	TMap<EJudgement, UTexture2D*> JudgementHighlightTexture;

	class AStageManager* StageManager;
};