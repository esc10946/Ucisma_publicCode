// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "System/CommonTypes.h"
#include "GameFramework/Actor.h"
#include "../Widget/Wiget_State.h"
#include "UIManager.generated.h"


UCLASS()
class UCISMA_API AUIManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUIManager();
	//스테이지 매니저에 의해 만들어진 후 생성자를 제외하고 가장 먼저 실행되는 함수
	void Init(class AStageManager* InputStageManager);
	//비트마다 호출되는 함수, QuaterIndex에는 박자를 1/4박자로 쪼갰을 때 몇 번째 1/4박자인지 알려줌
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);
	void UpdateCombo(int Value);


	/***Get 함수들***/
	class UWidget_BeatBar* GetBeatBarInstance();
	class UWidget_Purification* GetPurificationInstance();
	class UWidget_Score* GetScoreInstance();
	class UWidget_Combo* GetComboInstance() { return ComboInstance; }
	class UWidget_Loading* GetLoadingInstnace() { return LoadingInstance; }
	UFUNCTION(BlueprintCallable)
	class UWidget_TutorialText* GetTutorialText() { return TutorialTextWidget;  }

	// 게임 오버 위젯 레퍼런스
	UPROPERTY()
	UUserWidget* GameCompleteWidget;
	void ShowGameOverUI(EGameOverType GameOverType);
	void ShowTutorialUI();
	void GameComplete();
	void DeleteNote(EJudgementAction Action);
	void ShowJudgementHighlight(EJudgementResult Judgement);
	void ShowPurificationFailureCount(FVector2D ActorScreenPos);
	void ShowPause();
	void ToggleDebugMode(bool bIsActivate);
	void UpdateDebugMode();
	void PrintPlayerOutMessage();
	UFUNCTION()
	void HandleTutorialText(const FString& MainText, const FString& SubText);
	UFUNCTION()
	void ChangeTutorialKeyImage(ETutorialKeyType PressedKey);
	UFUNCTION()
	void ActiveUiTutorial(EUIType typeUi, bool active);

private:
	class AStageManager* StageManager;
	class UTutorialManager* TutorialManagerComp;
	//게임 플레이 HUD의 클래스
	TSubclassOf<class UWidget_GameplayHUD> GameplayHUDClass;
	//게임 플레이 HUD의 인스턴스
	class UWidget_GameplayHUD* GameplayHUDInstance;
	//정화 실패 시 화면에 출력되는 남은 정화 실패 횟수의 위젯 클래스
	TSubclassOf<class UWidget_Purification> PurificationClass;
	//정화 실패 시 화면에 출력되는 남은 정화 실패 횟수의 위젯 인스턴스
	class UWidget_Purification* PurificationInstance;
	//현재 콤보를 알려주는 텍스트의 위젯 클래스
	TSubclassOf<class UWidget_Loading> LoadingClass;
	//현재 콤보를 알려주는 텍스트의 위젯 인스턴스
	class UWidget_Loading* LoadingInstance;
	//노트가 보이는 비트바의 위젯 인스턴스
	class UWidget_BeatBar* BeatBarInstance;
	//노래 진행도가 보이는 노래 진행바의 위젯 인스턴스
	class UWidget_SongProgressBar* SongProgressBarInstance;
	//현재 점수를 알려주는 텍스트의 위젯 인스턴스
	class UWidget_Score* ScoreInstance;
	//현재 콤보를 알려주는 텍스트의 위젯 인스턴스
	class UWidget_Combo* ComboInstance;
	//현재 콤보를 알려주는 텍스트의 위젯 인스턴스
	class UWiget_State* StateInstance;

	//퍼즈버튼 위젯 클래스
	TSubclassOf<class UUserWidget> PauseButtonClass;
	//퍼즈버튼 위젯 인스턴스
	UUserWidget* PauseButtonInstance;

	//퍼즈창 위젯 클래스
	TSubclassOf<class UUserWidget> PauseClass;
	//퍼즈창 위젯 인스턴스
	UUserWidget* PauseInstance;
	//디버그 모드 위젯
	TSubclassOf<class UWidget_DebugMode> DebugClass;
	class UWidget_DebugMode* DebugInstance;
	// 게임 오버 위젯
	TSubclassOf<class UWidget_GameOver> GameOverWidgetClass;
	class UWidget_GameOver* GameOverWidget;
	//튜토리얼 텍스트
	TSubclassOf<UUserWidget> TutorialTextClass;
	class UWidget_TutorialText* TutorialTextWidget;
	TSubclassOf<class UWidget_PlayerOutMessage> PlayerOutMessageClass;
	class UWidget_PlayerOutMessage* PlayerOutMessageInstance;
	

	TMap<EUIType, UUserWidget*> UiSet;
private:
	TSubclassOf<class UWiget_State> StateClass;
};