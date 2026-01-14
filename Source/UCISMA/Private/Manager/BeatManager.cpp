// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager/BeatManager.h"
#include "Manager/StageManager.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Manager/UIManager.h"
#include "Widget/Widget_BeatBar.h"

#define MAX_QUATER_INDEX 3
#define SECONDS_PER_MINUTE 60.0f

// Sets default values
ABeatManager::ABeatManager()
{
	// 변수 초기화
	PrimaryActorTick.bCanEverTick = true;
	Bpm = 0;
	InputOffset = 0.0f;
	BeatIndex = 0;
	QuaterBeatIndex = -1;
	SecondPerBeat = 0.0f;
	BeatPerSecond = 0.0f;
	PerfectPercentageOfAttack = 0.0f;
	GreatPercentageOfAttack = 0.0f;
	GoodPercentageOfAttack = 0.0f;
	SuccessPercentageOfDash = 0.0f;
	LastBeatIndex = 0;
	bViewDebugMode = false;
	bCanActions.Emplace(EJudgementAction::Attack, false);
	bCanActions.Emplace(EJudgementAction::Dash, false);
}

// Called when the game starts or when spawned
void ABeatManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABeatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABeatManager::Init(AStageManager* InputStageManager)
{
	this->StageManager = InputStageManager;
	SetupStage();

	return true;
}

void ABeatManager::SetupStage()
{
	//변수 초기화
	this->Bpm = StageManager->GetBpm();
	this->InputOffset = StageManager->GetInputOffset();
	SecondPerBeat = SECONDS_PER_MINUTE / Bpm;
	BeatPerSecond = Bpm / SECONDS_PER_MINUTE;
	this->PerfectPercentageOfAttack = StageManager->GetPerfectPercentageOfAttack();
	this->GreatPercentageOfAttack = StageManager->GetGreatPercentageOfAttack();
	this->GoodPercentageOfAttack = StageManager->GetGoodPercentageOfAttack();
	this->SuccessPercentageOfDash = StageManager->GetSuccessPercentageOfDash();
	this->PerfectPercentageOfDash = StageManager->GetPerfectPercentageOfDash();
	LastBeatIndex = BeatPerSecond * StageManager->GetSongLength() + 4;
	bIsBeatUpdateStart = false;
}

void ABeatManager::StartBeat()
{
	FTimerHandle TimerHandle;

	// 1/4박자마다 BeatUpdate를 실행시킨다.
	GetWorld()->GetTimerManager().SetTimer(BeatTimerHandle, this, &ABeatManager::BeatUpdate, SecondPerBeat * 0.25f, true, StageManager->GetDelayForSongAndBeat() + InputOffset);
}

void ABeatManager::BeatUpdate()
{
	//만약 노래의 끝에 도달하면 세이브하고 더이상 비트 업데이트를 실행하지 않는다.
	if (CheckLastBeat())
	{
		return;
	}

	//비트 업데이트가 호출되기 시작하면 딱 한 번 OnBeatUpdateStart 호출
	if (!bIsBeatUpdateStart)
	{
		OnBeatUpdateStart();
		bIsBeatUpdateStart = true;
	}

	UpdateBeatIndex();
	if (QuaterBeatIndex == 0)
	{
		OnEveryBeat();
	}
	UpdateActionsActivation();
	//BeatUpdateListenrs에 등록된 모든 비트 업데이트를 실행한다.
	StageManager->BeatUpdateListeners.Broadcast(QuaterBeatIndex);
	float LogTime = StageManager->GetSecondPerBeat();
	if(bViewDebugMode) LOG_KHJ_SCREEN(LogTime, TEXT("Beat Index = %d, QuaterBeatIndex = %d"), BeatIndex, QuaterBeatIndex);
	/*if (BeatIndex ==40)
	{
		LOG_KHJ(Display, TEXT("Debug Pause at Beat %d"), BeatIndex);
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}*/
}

void ABeatManager::OnBeatUpdateStart()
{
	StageManager->ActivatePlayerInputIgnore(false);
}

void ABeatManager::OnEveryBeat()
{
	/*
	//비트마다 점수 추가
	if (StageManager->GetClearStrikeCount() > 0)
	{
		StageManager->AddScore(StageManager->GetNowCombo() * 10);
	}
	else if (StageManager->GetClearStrikeCount() == 0)
	{
		StageManager->AddScore(5);
	}
	*/
	StageManager->AddScore(10);

	//비트마다 메트로늄 소리 추가
	PlayMetronomeSound();
}

bool ABeatManager::CheckLastBeat()
{
	if (BeatIndex >= LastBeatIndex)
	{
		StageManager->GameOver(EGameOverType::GameClear_Normal);
		return true;
	}
	return false;
}

void ABeatManager::UpdateActionsActivation()
{
	//각 액션마다 가능 여부를 초기화한다. (각 액션의 타이밍 + 2/4박자)
	for (const TPair<EJudgementAction, int>& DelayQuaterBeat : StageManager->GetActionDelayQuaterBeat())
	{
		if (QuaterBeatIndex == (2 + DelayQuaterBeat.Value) % 4)
		{
			bCanActions[DelayQuaterBeat.Key] = true;
		}
	}
}

void ABeatManager::UpdateBeatIndex()
{
	//QuaterBeatIndex == 4 즉, 한 박자가 모두 지날 경우 비트 인덱스 갱신
	QuaterBeatIndex++;
	if (QuaterBeatIndex == 4)
	{
		QuaterBeatIndex = 0;
		BeatIndex++;
	}
}


EJudgementResult ABeatManager::CheckJudgement(EJudgementAction Action)
{
	//비트로부터 지난 시간, 남은 시간을 계산한다. 다만 BeatUpdate가 1/4비트마다 업데이트 되기에 QuaterBeatIndex * 0.25 * SecondPerBeat 과정을 거친다.
	float ElapsedTimeFromBeat = GetWorldTimerManager().GetTimerElapsed(BeatTimerHandle) + (SecondPerBeat * 0.25) * (QuaterBeatIndex - StageManager->GetActionDelayQuaterBeat()[Action]);
	float RemainingTimeFromBeat = GetWorldTimerManager().GetTimerRemaining(BeatTimerHandle) + SecondPerBeat * 0.25 * (MAX_QUATER_INDEX - (QuaterBeatIndex - StageManager->GetActionDelayQuaterBeat()[Action]));
	if (ElapsedTimeFromBeat < 0)	ElapsedTimeFromBeat += SecondPerBeat;
	if (RemainingTimeFromBeat > SecondPerBeat)	RemainingTimeFromBeat -= SecondPerBeat;
	RecentlyJudgementErrorTime = (ElapsedTimeFromBeat > RemainingTimeFromBeat) ? (-RemainingTimeFromBeat) : (ElapsedTimeFromBeat);

	LOG_PSJ_SCREEN_WITH_ID(2, 10.0f, TEXT("E : %f | R : %f"), ElapsedTimeFromBeat, RemainingTimeFromBeat);

	//Attack 판정 검사
	if (Action == EJudgementAction::Attack)
	{
		if (!bCanActions[Action])
		{
			return EJudgementResult::AttackFail;
		}
		bCanActions[Action] = false;
		if (StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Attack] == StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Dash])
		{
			bCanActions[EJudgementAction::Dash] = false;
		}
		float JudgementTime;
		//남은시간과 지난시간을 합친 범위가 Percentage이기 때문에 2를 나눠서 계산함
		JudgementTime = SecondPerBeat * PerfectPercentageOfAttack / 2;
		if (ElapsedTimeFromBeat < JudgementTime || RemainingTimeFromBeat < JudgementTime)
		{
			return EJudgementResult::AttackPerfect;
		}
		JudgementTime = SecondPerBeat * GreatPercentageOfAttack / 2;
		if (ElapsedTimeFromBeat < JudgementTime || RemainingTimeFromBeat < JudgementTime)
		{
			return EJudgementResult::AttackGreat;
		}
		JudgementTime = SecondPerBeat * GoodPercentageOfAttack / 2;
		if (ElapsedTimeFromBeat < JudgementTime || RemainingTimeFromBeat < JudgementTime)
		{
			return EJudgementResult::AttackGood;
		}
		return EJudgementResult::AttackNormal;

	}
	//Dash 판정 검사
	else if (Action == EJudgementAction::Dash)
	{
		if (!bCanActions[Action])
		{
			return EJudgementResult::DashFail;
		}
		bCanActions[Action] = false;
		if (StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Attack] == StageManager->GetActionDelayQuaterBeat()[EJudgementAction::Dash])
		{
			bCanActions[EJudgementAction::Attack] = false;
		}
		//남은시간과 지난시간을 합친 범위가 Percentage이기 때문에 2를 나눠서 계산함
		float JudgementTime;
		JudgementTime = SecondPerBeat * PerfectPercentageOfDash / 2;
		if (ElapsedTimeFromBeat < JudgementTime || RemainingTimeFromBeat < JudgementTime)
		{
			return EJudgementResult::DashPerfect;
		}
		JudgementTime = SecondPerBeat * SuccessPercentageOfDash / 2;
		if (ElapsedTimeFromBeat < JudgementTime || RemainingTimeFromBeat < JudgementTime)
		{
			return EJudgementResult::DashSuccess;
		}
	}
	return EJudgementResult::DashFail;
}

void ABeatManager::PlayMetronomeSound()
{
	StageManager->PlaySfx(ESfx::MetronomeSound);
}