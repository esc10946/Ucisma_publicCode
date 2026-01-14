#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Manager/StageManager.h"
#include "BeatManager.generated.h"





UCLASS()
class UCISMA_API ABeatManager : public AActor
{
	GENERATED_BODY()

public:
	/***********************************함수**************************************/
	/*** 흐름 ***/
	// 생성자
	ABeatManager();
	// 틱 함수
	virtual void Tick(float DeltaTime) override;
	//스테이지를 시작할 때 비트매니저가 해야 하는 설정 초기화
	bool Init(class AStageManager* InputStageManager);
	//Bpm에 맞추어 1/4비트마다 호출되는 함수
	void BeatUpdate();
	/*** 미분류 ***/
	//행동에 따라 판정을 내려주는 함수, 2~3번째 매개변수로 정박자로부터 지난시간/남은시간을 알 수 있다.
	EJudgementResult CheckJudgement(EJudgementAction Action);
	//싱크를 맞추기 위한 BeatUpdate 호출과 노래 시작 호출
	void StartBeat();
	
	/***Get 함수들***/
	int GetBpm() { return Bpm; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetBeatIndex() { return BeatIndex; }
	int GetQuaterBeatIndex() { return QuaterBeatIndex; }
	float GetSecondPerBeat() { return SecondPerBeat; }
	float GetBeatPerSecond() { return BeatPerSecond; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetLastBeatIndex() { return LastBeatIndex; }

	/***Set 함수들***/
	void SetPerfectPercentageOfAttack(float Value) { PerfectPercentageOfAttack = Value; }
	void SetPerfectPercentageOfDash(float Value) { PerfectPercentageOfDash = Value; }
	void SetBeatIndex(int Value) { BeatIndex = Value; }
	void SetViewDebugMode(bool activeMode) { bViewDebugMode = activeMode;  }

	/***********************************변수**************************************/
	//가장 최근 판정의 오차시간, 판정 보정 맵에서 활용
	UPROPERTY(BlueprintReadOnly)
	float RecentlyJudgementErrorTime;
	//BeatUpdate를 호출하는 타이머의 TimerHandle
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle BeatTimerHandle;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/***********************************함수**************************************/
	//스테이지 초기 설정
	void SetupStage();
	//비트 업데이트가 처음 호출될 때 실행되는 함수
	void OnBeatUpdateStart();
	//매 박자마다 (1/4박자 X) 호출되는 함수
	void OnEveryBeat();
	//마지막 박자 즉, 게임 종료인지 확인하는 함수
	bool CheckLastBeat();
	//특정 비트마다 특정 액션을 활성화하는 함수
	void UpdateActionsActivation();
	//비트 인덱스를 업데이트 하는 함수
	void UpdateBeatIndex();
	//메트로늄 소리를 내는 함수
	void PlayMetronomeSound();
	

	/***********************************변수**************************************/
	/*** 비트 관련 ***/
	//노래의 Bpm
	int Bpm;
	//현재 비트를 구분하는 인덱스, 정박자가 지난 횟수라고 생각하면 된다.
	UPROPERTY(EditAnywhere, Category = Beat)
	int BeatIndex;
	//비트를 1/4로 쪼갰을 때 몇번째인지 구분하는 인덱스, 한 비트당 0~3 값이 1/4비트마다 갱신된다.
	int QuaterBeatIndex;
	//한 비트에서 다음 비트까지 걸리는 시간, 비트당 시간
	float SecondPerBeat;
	//1초에 생기는 비트의 개수로 소수점까지 표기된다. 예를 들어 bpm이 90인 경우 1.5가 된다.
	float BeatPerSecond;
	//해당 노래의 길이를 비트로 환원한 것
	UPROPERTY(VisibleAnywhere, Category = Beat)
	int LastBeatIndex;

	/*** 판정 관련 ***/
	//여기서 BPM이 90이고 0.25인 경우 비트 하나당 0.67초이므로 0.67 * 0.25 * 0.5 = +-0.08375초가 판정 범위인 것이다.
	//공격 퍼펙트 판정 비율, 0.15인 경우 비트 시간의 15%까지는 퍼펙트로 인정하는 것이다.
	float PerfectPercentageOfAttack;
	//공격 그레이트 판정 비율, 0.25인 경우 비트 시간의 25%까지는 그레이트로 인정하는 것이다.
	float GreatPercentageOfAttack;
	//공격 굳 판정 비율, 0.35인 경우 비트 시간의 35%까지는 굳으로 인정하는 것이다.
	float GoodPercentageOfAttack;
	//퍼펙트 대쉬 성공 판정 비율, 0.25인 경우 비트 시간의 25%까지는 대쉬 성공으로 인정하는 것이다.
	float PerfectPercentageOfDash;
	//대쉬 성공 판정 비율, 0.35인 경우 비트 시간의 35%까지는 대쉬 성공으로 인정하는 것이다.
	float SuccessPercentageOfDash;
	//액션 가능 여부
	UPROPERTY(VisibleAnywhere)
	TMap<EJudgementAction, bool> bCanActions;
	

	/*** 미분류 ***/
	class AStageManager* StageManager;
	
	//플레이어가 설정하는 세부 판정, 정확한 정의는 stageManager 참고
	float InputOffset;
	bool bIsBeatUpdateStart;
	
	bool bViewDebugMode;
};