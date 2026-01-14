// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundWave.h"
#include "Kismet/GameplayStatics.h"

#include "System/CommonTypes.h"

#include "StageManager.generated.h"

//비트 업데이트 델리게이트 추가
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeatUpdateDelegate, int, QuaterIndex);

DECLARE_MULTICAST_DELEGATE(FOnPurificationSuccess); // 🔹 정화 성공 이벤트

// TODO: 플레이어 공격 성공 시 점수, 콤보 등의 변화를 함수 하나만 호출해도 해결할 수 있도록 리팩토링하기
// TODO: 함수와 변수 목록의 가독성 증진시키기

UCLASS()
class UCISMA_API AStageManager : public AActor
{
	GENERATED_BODY()

public:
	/********************************* 함수 ********************************************/


	/************************/
	/********* 흐름 *********/
	/************************/

	AStageManager();
	virtual void Tick(float DeltaTime) override;
	//스테이지가 시작될 때 호출되는 함수
	void InitStage();
	UFUNCTION()
	void StartBeatAndSong();
	bool IsLoadingComplete();


	/*************************/
	/********** Add **********/
	/*************************/

	//점수를 Value만큼 더함
	void AddScore(int Value);
	//ClearStrikeCount를 Value만큼 더함
	void AddClearStrikeCount(int Value);
	//콤보를 Value만큼 더함
	void AddCombo(int Value);


	/*************************/
	/********* 세이브 ********/
	/*************************/

	//현재 Score, ClearStrikeCount, Combo, PurificationSuccessCount를 세이브 파일에 저장한다.
	bool SaveStageRecord(bool iscomplete);
	//
	// Asset에서 데이터를 가져오는 함수
	UFUNCTION(CallInEditor, Category = "Data")
	void LoadGameDataAsset();

	
	/*************************/
	/********* 미분류 ********/
	/*************************/

	UFUNCTION(BlueprintCallable)
	void SetStagePaused(bool Paused);
	UFUNCTION(BlueprintCallable)
	void GameOver(EGameOverType GameOverType);
	UFUNCTION(BlueprintCallable)
	EJudgementResult CheckJudgement(EJudgementAction Action);
	UFUNCTION(BlueprintCallable)
	void ShowPurificationFailureCount(FVector2D ActorScreenPos);
	UFUNCTION(BlueprintCallable)
	void FailPurify(FVector2D ActorScreenPos);
	UFUNCTION(BlueprintCallable)
	void PlaySfx(ESfx SfxType);
	//포커스 여부에 따라 게임 일시정지 여부를 결정해주는 함수
	UFUNCTION(BlueprintCallable)
	void OnFocusChange(bool bIsActive);
	UFUNCTION(BlueprintCallable)
	void IncreasePurificationCount(int value); // 🔹 정화 성공 시 호출
	UFUNCTION(BlueprintCallable)
	void PlayAddScoreAnim(int Value);
	UFUNCTION(BlueprintCallable)
	void ShowPauseUI();

	bool IsBossStage() const; // 🔹 현재 스테이지가 보스 스테이지인지 확인
	void BossDeadHandler();

	//변수 값을 초기화하는 함수
	void InitVariables();

	UFUNCTION(BlueprintCallable)
	bool GetCameraLocationAtBeatIndex(int BeatIndex, FVector& OutLocation);
	void PrintPlayerOutMessage();

	/*************************/
	/********* Debug *********/
	/*************************/

	//디버그 모드 토글
	UFUNCTION(BlueprintCallable)
	void ToggleDebugMode();
	//공격 및 대쉬 판정을 전부 퍼펙트 판정으로 바꾸는 모드 토글
	UFUNCTION(BlueprintCallable)
	void ToggleAllPerfectMode();
	//정화 실패 제한을 무한으로 늘리는 모드 토글
	UFUNCTION(BlueprintCallable)
	void ToggleInfinityPurificationMode();
	UFUNCTION(BlueprintCallable)
	void ToggleInfinityHealthAndStaminaMode();
	UFUNCTION(BlueprintCallable)
	void ToggleInvincibilityMode();
	UFUNCTION(BlueprintCallable)
	void ToggleFixPlayerMode();
	UFUNCTION(BlueprintCallable)
	void ToggleInfinityDamage();
	UFUNCTION(BlueprintCallable)
	void ToggleViewBeatIndex();
	UFUNCTION(BlueprintCallable)
	void ToggleInfinityRange();

	UFUNCTION(BlueprintCallable)
	void ActivateDebugMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateAllPerfectMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateInfinityPurificationMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateInfinityHealthAndStaminaMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateInvincibilityMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateFixPlayerMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateInfinityDamage(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivatePlayerInputIgnore(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateViewBeatIndexMode(bool bActive);
	UFUNCTION(BlueprintCallable)
	void ActivateInfinityRange(bool bActive);
	


	/*************************/
	/********** Get **********/
	/*************************/

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class ABeatManager* GetBeatManager() { return BeatManager; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class AUIManager* GetUIManager() { return UIManager; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class AUnitManager* GetUnitManager() { return UnitManager; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class ASoundManager* GetSoundManager() { return SoundManager; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class UTutorialManager* GetTutorialManager() { return TutorialManager; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetBpm() {	return Bpm;	}
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetInputOffset() { return InputOffset; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetSongLength() { return SongLength; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	USoundWave* GetSong() { return Song; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetPerfectPercentageOfAttack() { return PerfectPercentageOfAttack; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetGreatPercentageOfAttack() { return GreatPercentageOfAttack; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetGoodPercentageOfAttack() { return GoodPercentageOfAttack; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetPerfectPercentageOfDash() { return PerfectPercentageOfDash; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetSuccessPercentageOfDash() { return SuccessPercentageOfDash; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetPurificationSuccessCount() { return PurificationSuccessCount; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetPurificationFailureCount() { return PurificationFailureCount; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetScore() { return Score; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetClearStrikeCount() { return ClearStrikeCount; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetMaxCombo() { return MaxCombo; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetNowCombo() { return NowCombo; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	EStageIndex GetStageIndex() { return StageIndex; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetMaxPurificationFailureCount() { return MaxPurificationFailureCount; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetBeatIndex();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetQuaterBeatIndex();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetDelayForSongAndBeat() { return DelayForSongAndBeat;	}
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetSecondPerBeat();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetBeatPerSecond(); 
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	int GetLastBeatIndex();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetPlayerRange() { return Player_Range; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetCameraSpeed() { return Camera_Speed; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetPlayerDashRange() { return Player_Dash_Range; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	class UGameDataAsset* GetGameDataAsset() { return GameDataAsset; }
	UFUNCTION(BlueprintCallable)
	class ACamera_Map* GetCamera() { return Camera; }
	int GetStageStartIndex() { return StageStartIndex; }
	float GetGameSpeed() { return GameSpeed; }
	TMap<EJudgementAction, int>& GetActionDelayQuaterBeat() { return ActionDelayQuaterBeat; }
	class APlayerCharacter* GetPlayer() { return Player; }
	//기준점수 계산함수
	float GetBaseScore(EStageIndex StageIndex);

	bool GetIsStartStage() { return bIsStartStage; }

	bool GetIsGamePaused() { return bIsGamePaused; }

	bool IsTutorialStage() const;

	/*************************/
	/********** Set **********/
	/*************************/

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetPurificationSuccessCount(int Value) { PurificationSuccessCount = Value; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetPurificationFailureCount(int Value) { PurificationFailureCount = Value; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetScore(int Value) { Score = Value; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetClearStrikeCount(int Value) { ClearStrikeCount = Value; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetMaxCombo(int Value) { MaxCombo = Value; }
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetNowCombo(int Value);
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetInputOffset(float Value) { InputOffset = Value; }

	/********************************* 변수 ********************************************/
	//비트 업데이트를 받고 싶은 델리게이트를 할당한 변수
	UPROPERTY(BlueprintAssignable)
	FBeatUpdateDelegate BeatUpdateListeners;
	FOnPurificationSuccess OnPurificationSuccess; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DebugMode")
	TMap<EDebugMode, bool> DebugModeToggles;



protected:
	virtual void BeginPlay() override;



private:
	/********************************* 함수 ********************************************/
	//스테이지의 공통 사항을 설정하는 함수
	void SetupStageBase();
	//스테이지 별 특수 사항을 설정하는 함수
	void SetupStageSpecial();
	//각종 매니저들을 생성 및 Init하는 함수
	void InitManagers();
	//중간 시작에 대한 설정을 하는 함수
	void SetupStageSkip();
	int GetEnemyCount();
	//두 수가 들어오면 퍼센트로 바꿔주는 함수
	int GetPercentageBetween(float Min, float Max, float Value);
	//좌표와 퍼센트로 보간된 좌표를 반환하는 함수
	FVector GetPositionBetweenByPercent(const FVector& A, const FVector& B, float Percent);

	/********************************* 변수 ********************************************/


	/************************/
	/******** 매니저 ********/
	/************************/

	//생성된 비트 매니저
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class ABeatManager* BeatManager;
	//생성된 UI 매니저
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class AUIManager* UIManager;
	//생성된 Unit 매니저
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class AUnitManager* UnitManager;
	//생성된 Sound 매니저
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class ASoundManager* SoundManager;
	//생성된 비트 매니저
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class UTutorialManager* TutorialManager;
	UPROPERTY(VisibleAnywhere, Category = "Manager")
	class UMyGameInstance* GameInstance;


	/************************/
	/******** 시스템 ********/
	/************************/
	//게임의 기본적인 데이터가 담긴 게임 데이터 에셋
	UPROPERTY(VisibleAnywhere, Category = "System")
	class UGameDataAsset* GameDataAsset;
	//현재 게임이 pause 상태인지 확인하는 변수
	UPROPERTY(VisibleAnywhere, Category = "System")
	bool bIsGamePaused;
	UPROPERTY(EditInstanceOnly, Category = "System")
	int StageStartIndex;
	UPROPERTY()
	int StageStartBeatIndex;
	UPROPERTY(EditInstanceOnly, Category = "System")
	float GameSpeed;
	UPROPERTY()
	bool bIsInitStageComplete = false;
	UPROPERTY()
	bool bIsStartStage = false;
	

	/************************/
	/********* 노래 *********/
	/************************/

	//스테이지에서 재생되는 노래
	UPROPERTY(VisibleAnywhere, Category = "Music")
	USoundWave* Song;
	//스테이지에서 재생되는 노래의 BPM
	UPROPERTY(VisibleAnywhere, Category = "Music")
	int Bpm;
	//스테이지에서 재생되는 노래의 길이
	UPROPERTY(VisibleAnywhere, Category = "Music")
	int SongLength; // 단위는 초로 한다.
	float SecondPerBeat;
	float BeatPerSecond;


	/************************/
	/********* 판정 *********/
	/************************/

	//공격의 퍼펙트 판정 퍼센트
	UPROPERTY(VisibleAnywhere, Category = "Judgement")
	float PerfectPercentageOfAttack;
	//공격의 그레이트 판정 퍼센트
	UPROPERTY(VisibleAnywhere, Category = "Judgement")
	float GreatPercentageOfAttack;
	//공격의 굳 판정 퍼센트
	UPROPERTY(VisibleAnywhere, Category = "Judgement")
	float GoodPercentageOfAttack;
	//대쉬의 퍼펙트 성공 판정 퍼센트
	UPROPERTY(VisibleAnywhere, Category = "Judgement")
	float PerfectPercentageOfDash;
	//대쉬 성공 판정 퍼센트
	UPROPERTY(VisibleAnywhere, Category = "Judgement")
	float SuccessPercentageOfDash;
	UPROPERTY(EditAnywhere, Category = "Judgement")
	TMap<EJudgementAction, int> ActionDelayQuaterBeat;
	
	UPROPERTY(EditAnywhere, Category = "Setting")
	float InputOffset;

	UPROPERTY(VisibleAnywhere, Category = "Setting")
	float Player_Range;

	UPROPERTY(VisibleAnywhere, Category = "Setting")
	float Player_Dash_Range;

	UPROPERTY(VisibleAnywhere, Category = "Setting")
	float Camera_Speed;


	/************************/
	/********* 기록 *********/
	/************************/

	//스테이지 구분용 변수
	UPROPERTY(EditAnywhere, Category = "Record")
	EStageIndex StageIndex;
	//정화 성공 횟수
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int PurificationSuccessCount;
	//정화 실패 횟수
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int PurificationFailureCount;
	//최대 정화 실패 횟수
	UPROPERTY(VisibleAnywhere, Category = "Data")
	int MaxPurificationFailureCount;
	//점수
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int Score;
	//퍼펙트 공격 즉, ClearStrike의 횟수
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int ClearStrikeCount;
	//현재 플레이 중 최고 콤보
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int MaxCombo;
	//현재 플레이 중 현재 콤보
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int NowCombo;
	//기록들을 저장할 세이브파일 슬롯의 이름
	UPROPERTY(VisibleAnywhere, Category = "Record")
	FString SaveSlotName;
	
	UPROPERTY(VisibleAnywhere, Category = "Record")
	float BaseScore;
	UPROPERTY(VisibleAnywhere, Category = "Record")
	int EnemyCount = 0;
	//정화실패 구현 여부
	UPROPERTY(VisibleAnywhere, Category = "Data")
	bool bIsPurification;

	/*************************/
	/********* 미분류 ********/
	/*************************/

	class APlayerCharacter* Player;
	class ACamera_Map* Camera;
	class AMap_Array_Actor* MapArray;
	//InputOffset에 따라 BeatUpdate 시작과 노래 시작의 타이밍이 +-로 바뀌므로 필요하다.
	float DelayForSongAndBeat;
	//보스 유닛 정보
	class ABeatUnitCharacter_Boss_Harmyth* Boss;
};
