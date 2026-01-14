// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_BeatBar.h"
#include "Components/Slider.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"
#include "Components/Image.h"


void UWidget_BeatBar::Init(AStageManager* InputStageManager)
{
	this->StageManager = InputStageManager;
	//노트로 사용되는 슬라이드를 할당
	AttackNotes[0] = Cast<USlider>(GetWidgetFromName(TEXT("LeftNote_0")));
	AttackNotes[1] = Cast<USlider>(GetWidgetFromName(TEXT("LeftNote_1")));
	DashNotes[0] = Cast<USlider>(GetWidgetFromName(TEXT("RightNote_0")));
	DashNotes[1] = Cast<USlider>(GetWidgetFromName(TEXT("RightNote_1")));
	AttackJudgementHighlight = Cast<UImage>(GetWidgetFromName(TEXT("AttackHighlight")));
	DashJudgementHighlight = Cast<UImage>(GetWidgetFromName(TEXT("DashHighlight")));
	//모든 노트를 보이지 않게 초기화
	for (int i = 0; i < NOTE_SLIDER_COUNT; i++)
	{
		AttackNotes[i]->SetValue(0.0f);
		AttackNotes[i]->SetVisibility(ESlateVisibility::Hidden);
		AttackNoteValues[i] = -1.f;
		DashNotes[i]->SetValue(0.0f);
		DashNotes[i]->SetVisibility(ESlateVisibility::Hidden);
		DashNoteValues[i] = -1.f;
	}
	//BeatManager의 BeatUpdate에서 해당 클래스의 BeatUpdate를 호출하도록 설정
	StageManager->BeatUpdateListeners.AddDynamic(this, &UWidget_BeatBar::BeatUpdate);
	Bpm = StageManager->GetBpm();
}

void UWidget_BeatBar::BeatUpdate(int QuaterIndex)
{
	if (QuaterIndex == StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Attack])
	{
		SpawnAttackNote();
	}
	if (QuaterIndex == StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Dash])
	{
		SpawnDashNote();
	}
}

void UWidget_BeatBar::SpawnAttackNote()
{
	static int SpawnNoteIndex;
	SpawnNoteIndex++;
	//좌우 노트를 보이게 하고 1.0으로 값 변경
	AttackNoteValues[SpawnNoteIndex % NOTE_SLIDER_COUNT] = 1.0f;
	AttackNotes[SpawnNoteIndex % NOTE_SLIDER_COUNT]->SetValue(1.0f);
	AttackNotes[SpawnNoteIndex % NOTE_SLIDER_COUNT]->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_BeatBar::SpawnDashNote()
{
	static int SpawnNoteIndex;
	SpawnNoteIndex++;
	//좌우 노트를 보이게 하고 1.0으로 값 변경
	DashNoteValues[SpawnNoteIndex % NOTE_SLIDER_COUNT] = 1.0f;
	DashNotes[SpawnNoteIndex % NOTE_SLIDER_COUNT]->SetValue(1.0f);
	DashNotes[SpawnNoteIndex % NOTE_SLIDER_COUNT]->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_BeatBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateNote(InDeltaTime);
}

void UWidget_BeatBar::UpdateNote(float DeltaTime)
{
	float BeatPerSecond = StageManager->GetBeatPerSecond();

	for (int i=0; i<NOTE_SLIDER_COUNT; i++)
	{
		// Bpm/60.0 * DeltaTime만큼 감소하여 한 비트 시간 후에 노트가 가운데로 모이게 설정
		AttackNoteValues[i] -= BeatPerSecond * DeltaTime;
		AttackNotes[i]->SetValue(FMath::Max(0.0f, AttackNoteValues[i]));
		if (AttackNotes[i]->GetValue() <= 0.0f)
		{
			AttackNotes[i]->SetVisibility(ESlateVisibility::Hidden);
		}

		DashNoteValues[i] -= BeatPerSecond * DeltaTime;
		DashNotes[i]->SetValue(FMath::Max(0.0f, DashNoteValues[i]));
		if (DashNotes[i]->GetValue() <= 0.0f)
		{
			DashNotes[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidget_BeatBar::DeleteNote(EJudgementAction Action)
{
	bool bIsDelaySame = StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Attack] == StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Dash];
	//Value가 Min인 값을 찾아야 하므로 초기 값을 0이 아닌 1로 두었다.
	float TargetNoteValue = 1.0f;
	int TargetNoteIndex = 0;

	if (Action == EJudgementAction::Attack || bIsDelaySame)
	{
		for (int i = 0; i < NOTE_SLIDER_COUNT; i++)
		{
			if (TargetNoteValue > FMath::Abs(AttackNoteValues[i]))
			{
				TargetNoteValue = FMath::Abs(AttackNoteValues[i]);
				TargetNoteIndex = i;
			}
		}
		AttackNotes[TargetNoteIndex]->SetVisibility(ESlateVisibility::Hidden);
	}

	TargetNoteValue = 1.0f;
	TargetNoteIndex = 0;
	if(Action == EJudgementAction::Dash || bIsDelaySame)
	{
		for (int i = 0; i < NOTE_SLIDER_COUNT; i++)
		{
			if (TargetNoteValue > FMath::Abs(DashNoteValues[i]))
			{
				TargetNoteValue = FMath::Abs(DashNoteValues[i]);
				TargetNoteIndex = i;
			}
		}
		DashNotes[TargetNoteIndex]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_BeatBar::ShowJudgementHighlight(EJudgementResult Judgement)
{
	if (!AttackJudgementHighlight || !DashJudgementHighlight)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Error] Widget_BeatBar > ShowJudmentHighlight > Not Allocated Highlight Animation"));
		return;
	}
	//각 판정에 따라 하이라이트 애니메이션 실행
	switch (Judgement)
	{
	case EJudgementResult::AttackPerfect:
		AttackJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::AttackPerfect]);
		PlayAnimation(AnimAttackHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::AttackGreat:
		AttackJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::AttackGreat]);
		PlayAnimation(AnimAttackHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::AttackGood:
		AttackJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::AttackGood]);
		PlayAnimation(AnimAttackHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::AttackNormal:
		AttackJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::AttackNormal]);
		PlayAnimation(AnimAttackHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::DashSuccess:
		DashJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::DashSuccess]);
		PlayAnimation(AnimDashHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::DashPerfect:
		DashJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::DashSuccess]);
		PlayAnimation(AnimDashHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	case EJudgementResult::DashFail:
		DashJudgementHighlight->SetBrushFromTexture(JudgementHighlightTexture[EJudgement::DashFail]);
		PlayAnimation(AnimDashHighlight, 0.f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
		break;
	}
}