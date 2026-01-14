#include "Manager/UIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widget/Widget_BeatBar.h"
#include "Widget/Wiget_State.h"
#include "Components/Slider.h"
#include "Manager/StageManager.h"
#include "Manager/TutorialManager.h"
#include "Widget/Widget_SongProgressBar.h"
#include "Widget/Widget_Purification.h"
#include "Widget/Widget_Score.h"
#include "Widget/Widget_Combo.h"
#include "Widget/Widget_Loading.h"
#include "Widget/Widget_GameplayHUD.h"
#include "Widget/Widget_DebugMode.h"
#include "Widget/Widget_GameOver.h"
#include "Widget/Widget_TutorialText.h"
#include "Widget/Widget_PlayerOutMessage.h"
// TODO: UI의 Get함수들 전부 생성 후

// Sets default values
AUIManager::AUIManager()
{
	//변수 초기화
	PrimaryActorTick.bCanEverTick = true;
	//위젯 클래스 탐색
	static ConstructorHelpers::FClassFinder<UWidget_GameplayHUD> WidgetFinder_GameplayHUD(TEXT("/Game/UI/HUD/WBP_GameplayHUD"));
	if (WidgetFinder_GameplayHUD.Succeeded())
	{
		GameplayHUDClass = WidgetFinder_GameplayHUD.Class;
	}
	else
	{
		LOG_DEBUG(Error, TEXT("GameplayHUD FClassFinder failed"));
	}
	static ConstructorHelpers::FClassFinder<UWidget_Purification> WidgetFinder_Purification(TEXT("/Game/UI/WBP_Purification"));
	if (WidgetFinder_Purification.Succeeded())
	{
		PurificationClass = WidgetFinder_Purification.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Error]UIManager > Constructure > FClassFinder_Purification"));
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder_PuaseButton(TEXT("/Game/UI/Menu/WB_PauseButtonInGame"));
	if (WidgetFinder_PuaseButton.Succeeded())
	{
		PauseButtonClass = WidgetFinder_PuaseButton.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("FClassFinder_PauseButton Failed"));
	}

	static ConstructorHelpers::FClassFinder<UWidget_GameOver> WidgetFinder(TEXT("/Game/UI/Menu/WBP_GameOver"));
	if (WidgetFinder.Succeeded())
	{
		GameOverWidgetClass = WidgetFinder.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("[Error]Failed to load GameOverWidgetClass"));
	}
	static ConstructorHelpers::FClassFinder<UWidget_Loading> WidgetFinder_Loading(TEXT("/Game/UI/Menu/WB_Loading"));
	if (WidgetFinder_Loading.Succeeded())
	{
		LoadingClass = WidgetFinder_Loading.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("[Error]Failed to load LoadingtClass"));
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder_Pause(TEXT("/Game/UI/Menu/WB_Pause"));
	if (WidgetFinder_Pause.Succeeded())
	{
		PauseClass = WidgetFinder_Pause.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("[Error]Failed to load PauseClass"));
	}
	static ConstructorHelpers::FClassFinder<UWidget_DebugMode> WidgetFinder_Debug(TEXT("/Game/UI/HUD/DebugMode/WBP_DebugMode"));
	if (WidgetFinder_Debug.Succeeded())
	{
		DebugClass = WidgetFinder_Debug.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("Failed to load DebugClass"));
	}
	static ConstructorHelpers::FClassFinder<UWidget_TutorialText> WidgetFinderTutorial(TEXT("/Game/UI/WB_TutorialText.WB_TutorialText_C"));
	if (WidgetFinderTutorial.Succeeded()) {
		TutorialTextClass = WidgetFinderTutorial.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("Failed to load TutorialTextClass"));
	}
	static ConstructorHelpers::FClassFinder<UWidget_PlayerOutMessage> WidgetFinderPlayerOutMessage(TEXT("/Game/UI/HUD/WBP_PlayerOutMessage"));
	if (WidgetFinderPlayerOutMessage.Succeeded()) {
		PlayerOutMessageClass = WidgetFinderPlayerOutMessage.Class;
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("Failed to load PlayerOutMessageClass"));
	}
}

// Called when the game starts or when spawned
void AUIManager::Init(AStageManager* InputStageManager)
{
	//위젯 인스턴스 선언 및 출력
	StageManager = InputStageManager;

	if (StageManager->GetStageIndex() != EStageIndex::AdjustInputOffset)
	{
		GameplayHUDInstance = CreateWidget<UWidget_GameplayHUD>(GetWorld(), GameplayHUDClass);
		if (GameplayHUDInstance)
		{
			GameplayHUDInstance->AddToViewport(10);
			GameplayHUDInstance->Init(StageManager);
			SongProgressBarInstance = GameplayHUDInstance->SongProgressBar;
			BeatBarInstance = GameplayHUDInstance->BeatBar;
			ScoreInstance = GameplayHUDInstance->Score;
			ComboInstance = GameplayHUDInstance->Combo;
			StateInstance = GameplayHUDInstance->State;
		}
		PurificationInstance = CreateWidget<UWidget_Purification>(GetWorld(), PurificationClass);
		if (PurificationInstance)
		{
			PurificationInstance->AddToViewport(11);
			PurificationInstance->Init(StageManager);
		}
		else
		{
			LOG_DEBUG(Warning, TEXT("Pause Button Instance can't Create Widget"));
		}
		PauseButtonInstance = CreateWidget<UUserWidget>(GetWorld(), PauseButtonClass);
		if (PauseButtonInstance)
		{
			PauseButtonInstance->AddToViewport(99);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Error] UIManager > Init > Can't cast PurificationInstance"));
		}
		PlayerOutMessageInstance = CreateWidget<UWidget_PlayerOutMessage>(GetWorld(), PlayerOutMessageClass);
		if (PlayerOutMessageInstance)
		{
			PlayerOutMessageInstance->AddToViewport(99);
		}
		else
		{
			LOG_DEBUG(Warning, TEXT("Player Out Message Instance can't Create Widget"));
		}


	}
	if (StageManager->IsTutorialStage()) {
		TutorialManagerComp = StageManager->GetTutorialManager();
		TutorialManagerComp->OnRequestTutorialText.AddDynamic(this, &AUIManager::HandleTutorialText);
		TutorialManagerComp->OnTutorialKeyPressed.AddDynamic(this, &AUIManager::ChangeTutorialKeyImage);
		TutorialManagerComp->OnTutorialUIActive.AddDynamic(this, &AUIManager::ActiveUiTutorial);
		ShowTutorialUI();
	}
	StageManager->BeatUpdateListeners.AddDynamic(this, &AUIManager::BeatUpdate);
}

void AUIManager::BeatUpdate(int QuaterIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("UIManager > BeatUpdate > Execute!, QuaterIndex : %d"), QuaterIndex);
}

void AUIManager::UpdateCombo(int Value)
{
	ComboInstance->UpdateCombo(Value);
}



UWidget_BeatBar* AUIManager::GetBeatBarInstance()
{
	return BeatBarInstance;
}

UWidget_Purification* AUIManager::GetPurificationInstance()
{
	return PurificationInstance;
}

UWidget_Score* AUIManager::GetScoreInstance()
{
	return ScoreInstance;
}

void AUIManager::ShowGameOverUI(EGameOverType GameOverType) 
{
	GameOverWidget = CreateWidget<UWidget_GameOver>(GetWorld(), GameOverWidgetClass);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport(200);
		GameOverWidget->Init(StageManager);
		GameOverWidget->ShowGameOverUI(GameOverType);
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("GameOverWidget didn't called"));
	}
}

void AUIManager::ShowTutorialUI()
{
	if (!IsValid(TutorialTextClass))
		TutorialTextClass = LoadClass<UWidget_TutorialText>(nullptr, TEXT("WidgetBlueprint'/Game/UI/WB_TutorialText.WB_TutorialText_C'"));
	
	if (IsValid(TutorialTextClass)) {
		//시발 튜토리얼 위젯을 만들어야함 조짐;;
		TutorialTextWidget = CreateWidget<UWidget_TutorialText>(GetWorld(), TutorialTextClass);
		if (TutorialTextWidget) {
			TutorialTextWidget->AddToViewport();
			TutorialTextWidget->Init(StageManager);
			//LOG_KHJ_SCREEN(1.f, TEXT("Suceed"));
		}
	}

	UUserWidget* SongProgressBar = Cast<UUserWidget>(SongProgressBarInstance);
	UUserWidget* BeatBar = Cast<UUserWidget>(BeatBarInstance);
	UUserWidget* Combo = Cast<UUserWidget>(ComboInstance);
	UUserWidget* State = Cast<UUserWidget>(StateInstance);
	UUserWidget* Score = Cast<UUserWidget>(ScoreInstance);

	UiSet.Emplace(EUIType::SongProcessBar, SongProgressBar);
	UiSet.Emplace(EUIType::BeatBar, BeatBar);
	UiSet.Emplace(EUIType::Combo, Combo);
	UiSet.Emplace(EUIType::Score, Score);
	UiSet.Emplace(EUIType::State, State);

	for (uint8 i = 0; i <=static_cast<uint8>(EUIType::State); ++i) {
		EUIType currentUi = static_cast<EUIType>(i);
		if (UiSet.Contains(currentUi)) {
			UiSet[currentUi]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AUIManager::DeleteNote(EJudgementAction Action)
{
	if (BeatBarInstance)
	{
		BeatBarInstance->DeleteNote(Action);
	}
}

void AUIManager::ShowJudgementHighlight(EJudgementResult Judgement)
{
	if (BeatBarInstance)
	{
		BeatBarInstance->ShowJudgementHighlight(Judgement);
	}
}

void AUIManager::ShowPurificationFailureCount(FVector2D ActorScreenPos)
{
	if (PurificationInstance)
	{
		PurificationInstance->ShowPurificationFailureCount(ActorScreenPos);
	}
}

void AUIManager::ShowPause()
{
	PauseInstance = CreateWidget<UUserWidget>(GetWorld(), PauseClass);
	if (PauseInstance)
	{
		PauseInstance->AddToViewport(150);
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("PauseInstance not existed"))
	}
}

void AUIManager::ToggleDebugMode(bool bIsActivate)
{
	if (bIsActivate)
	{
		DebugInstance = CreateWidget<UWidget_DebugMode>(GetWorld(), DebugClass);
		DebugInstance->AddToViewport();
		DebugInstance->Init(StageManager);
	}
	else if(DebugInstance)
	{
		DebugInstance->RemoveFromParent();
		DebugInstance = nullptr;
	}
	UpdateDebugMode();
}

void AUIManager::UpdateDebugMode()
{
	if (DebugInstance)
	{
		DebugInstance->UpdateDebugMode();
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("UI_DebugInstance Not Existed but, call Update"));
	}
}

void AUIManager::HandleTutorialText(const FString& Title, const FString& Subtitle)
{
	// 로그로 확인
	UE_LOG(LogTemp, Display, TEXT("UIManager Received Tutorial Text: %s / %s"), *Title, *Subtitle);

	// 위젯이 존재하면 텍스트 전달
	if (TutorialTextWidget)
	{
		TutorialTextWidget->OutputTutorialText(
			FText::FromString(Title),
			FText::FromString(Subtitle)
		);
	}
}

void AUIManager::ChangeTutorialKeyImage(ETutorialKeyType PressedKey)
{
	const UEnum* EnumPtr = StaticEnum<ETutorialKeyType>();
	if (EnumPtr)
	{
		FText DisplayName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(PressedKey));
	}

	TutorialTextWidget->OnPressedKeyRight(PressedKey);
}

void AUIManager::ActiveUiTutorial(EUIType typeUi, bool bActive)
{
	if (!UiSet.Contains(typeUi) || !UiSet[typeUi])
	{
		//LOG_KHJ(Warning, TEXT("⚠️ UIManager: 타입 %d 의 UI가 존재하지 않음"), static_cast<uint8>(typeUi));
		return;
	}

	if (bActive) UiSet[typeUi]->SetVisibility(ESlateVisibility::Visible);
}

void AUIManager::PrintPlayerOutMessage()
{
	UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::ReturnNull);
	if (IsValid(PlayerOutMessageInstance))
	{
		PlayerOutMessageInstance->PrintPlayerOutMessage(1.0f);
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("Player out Message Instance didn't called"));
	}
}
