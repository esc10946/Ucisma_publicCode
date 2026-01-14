// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/TutorialManager.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "BeatUnit/BeatUnitActor_Laser.h"
#include "BeatUnit/BeatUnitActor_Obstacles.h"
#include "Components/CapsuleComponent.h"
#include "Manager/StageManager.h"
#include "PlayerCharacter.h"

// Sets default values for this component's properties
UTutorialManager::UTutorialManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bCanNextStep = true;
	tutorialBeatIndex = 0;
	currentStepEndIndex = 7;
	currentStepStartIndex = 0;
	attackCount = 0;
	attackCountStep8 = 0;
	currentStep = ETutorialStep::Step0;
	EnemyClass = AEnemyBase::StaticClass();
	LaserClass = ABeatUnitActor_Laser::StaticClass();
	ObstacleClass = ABeatUnitActor_Obstacles::StaticClass();
	// ...
}

void UTutorialManager::SetCurrentStep(ETutorialStep value)
{
	if (!TutorialDataSet)
	{
		UE_LOG(LogTemp, Error, TEXT("SetCurrentStep: TutorialDataSet is null!"));
		return;
	}
	currentStep = value;

	FTutorialStepData currentStepData = TutorialDataSet->GetStepData(currentStep);

	int startBeatIndex = currentStepData.FStepBeatIndexs.StartBeatIndex;
	int endBeatIndex = currentStepData.FStepBeatIndexs.EndBeatIndex;
	if (startBeatIndex < 0 || endBeatIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentStep: Invalid BeatIndex for Step %d"), (int32)currentStep);
		return;
	}

	currentStepStartIndex = startBeatIndex;
	currentStepEndIndex = endBeatIndex;

	const FTutorialTextData TextData = currentStepData.TextData;
	mainText = TextData.MainText;
	subText = TextData.SubText;

	bCanNextStep = !currentStepData.bNeedToLoop;
}

// Called when the game starts
void UTutorialManager::BeginPlay()
{
	Super::BeginPlay();
	// ...

}

// Called every frame
void UTutorialManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTutorialManager::Init(AStageManager* InputStageManager)
{
	this->stageManager = InputStageManager;
	stageManager->BeatUpdateListeners.AddDynamic(this, &UTutorialManager::BeatUpdate);

	PressedKeys.Emplace(ETutorialKeyType::W);
	PressedKeys.Emplace(ETutorialKeyType::A);
	PressedKeys.Emplace(ETutorialKeyType::S);
	PressedKeys.Emplace(ETutorialKeyType::D);
	PressedDashes.Emplace(EJudgementResult::DashSuccess);
	PressedDashes.Emplace(EJudgementResult::DashPerfect);
	OnTutorialConnectEnemy();
	return;
}

void UTutorialManager::LoadGameDataAsset()
{
	TutorialDataSet = LoadObject<UTutorialStageDataAsset>(
		nullptr,
		TEXT("/Game/Data/TutorialDataAsset.TutorialDataAsset")
	);

	if (!TutorialDataSet)
	{
		LOG_KHJ(Error, TEXT("❌ tutorialAssetLoading Failed!"));
		return;
	}

	SetCurrentStep(ETutorialStep::Step0);
}

void UTutorialManager::BeatUpdate(int QuaterIndex)
{
	if (QuaterIndex != 0) return;
	//튜토 강제 종료 조건
	if (stageManager->GetBeatIndex() * stageManager->GetSecondPerBeat() >= 555) {
		stageManager->SetStagePaused(true);
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
	}else if (stageManager->GetBeatIndex() * stageManager->GetSecondPerBeat() >= 490) {
		OnTutorialWarningAnimation.Broadcast();
	}

	//LOG_KHJ(Error, TEXT("현재 BeatIndex = %d"), tutorialBeatIndex);
	// 현재 스텝의 시작 타이밍이면 UI 출력
	if (tutorialBeatIndex == currentStepEndIndex+1)
	{
		if (GoNextStep()) {
			if(currentStep == ETutorialStep::Step9)
			{
				APlayerCharacter* Player = stageManager->GetPlayer();
				Player->Take_HP(-1, stageManager);
				Player->Player_Current_Health = 0;
			}
			//텍스트 출력및 UI제어
			ShowTutorialUI();
			ActiveUserWidget();
		}
		else {
			LOG_KHJ(Error, TEXT("스텝을 넘어가지 못함"));
		}
	}
	//튜토리얼 자체 인덱스와 엔드트리거 인덱스가 같으면 루프하는지 확인하기
	if (tutorialBeatIndex == currentStepEndIndex) {
		if (currentStep == ETutorialStep::StepFinal)
		{
			stageManager->SetStagePaused(true);
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
		}
			
		//루프 조건이 맞다면 StageManager에서 bCanNextStep를 수정하기
		if (!bCanNextStep) {
			//각 스텝마다 차이를 tutorialBeatIndex에서 뺄것 -> loop하게 만듦
			int32 differenceSteps = currentStepEndIndex - currentStepStartIndex + 1;
			tutorialBeatIndex -= differenceSteps;
			LOG_KHJ(Error, TEXT("루프함"));
			if (static_cast<uint8>(currentStep) < static_cast<uint8>(ETutorialStep::Step5)) {
				TutorialObstacles->SetBeatIndex(TutorialObstacles->GetBeatIndex() + differenceSteps );
				TutorialObstacles->SetEndBeatIndex(TutorialObstacles->GetEndBeatIndex() + differenceSteps );
				TutorialLaser1->SetBeatIndex(TutorialLaser1->GetBeatIndex() + differenceSteps );
				TutorialLaser2->SetBeatIndex(TutorialLaser2->GetBeatIndex() + differenceSteps);
			}
			if (static_cast<uint8>(currentStep) < static_cast<uint8>(ETutorialStep::Step10)) {
				TutorialEnemy1->SetBeatIndex(TutorialEnemy1->GetBeatIndex() + differenceSteps );
				TutorialEnemy2->SetBeatIndex(TutorialEnemy2->GetBeatIndex() + differenceSteps );
			}
		}
	}
	tutorialBeatIndex++;
}

bool UTutorialManager::GoNextStep()
{
	if (currentStep == ETutorialStep::StepFinal) {
		LOG_KHJ(Error, TEXT("UnBound Step. It's FinalStep"));
		return false;
	}


	ETutorialStep NextStep = static_cast<ETutorialStep>(static_cast<uint8>(currentStep) + 1);
	SetCurrentStep(NextStep);

	return true;
}

bool UTutorialManager::GoPreviousStep()
{
	if (currentStep == ETutorialStep::Step0) {
		LOG_KHJ(Error, TEXT("UnBound Step. It's 0Step"));
		return false;
	}
	ETutorialStep PreviousStep = static_cast<ETutorialStep>(static_cast<uint8>(currentStep) - 1);
	SetCurrentStep(PreviousStep);

	return true;
}

void UTutorialManager::ShowTutorialUI()
{
	// UImanager를 바인딩하거나 델리게이트 사용해서 TutorialText에 접근하기
	//LOG_KHJ_SCREEN(2.f, TEXT("MainText: %s"), *mainText.ToString());
	//LOG_KHJ_SCREEN(2.f, TEXT("SubText: %s"), *subText.ToString());

	OnRequestTutorialText.Broadcast(*mainText.ToString(), *subText.ToString());
	OnTutorialStepChanged.Broadcast(currentStep);
}

void UTutorialManager::HideTutorial()
{

}

void UTutorialManager::OnTutorialKeyInput(ETutorialKeyType KeyCode)
{
	if (currentStep == ETutorialStep::Step4) {
		if (!PressedKeys[KeyCode])
		{
			PressedKeys[KeyCode] = true;

			// 🔔 UI에 불 켜라고 델리게이트 보내기
			OnTutorialKeyPressed.Broadcast(KeyCode);
		}
		if (AreAllTutorialKeysPressed()) {
			bCanNextStep = true;
		}
	}
}

bool UTutorialManager::AreAllTutorialKeysPressed() const
{
	for (uint8 i = 0; i <= static_cast<uint8>(ETutorialKeyType::D); ++i)
	{
		ETutorialKeyType Key = static_cast<ETutorialKeyType>(i);

		bool bPressed = PressedKeys[Key];
		if (!bPressed)
		{
			return false; // 하나라도 안 눌렸으면 false
		}
	}
	return true; // 전부 눌림
}

void UTutorialManager::OnTutorialDashInput(EJudgementResult dashjudge)
{
	if (currentStep == ETutorialStep::Step6) {
		if (PressedDashes.Contains(dashjudge)&&!PressedDashes[dashjudge])
		{
			PressedDashes[dashjudge] = true;
			// 🔔 UI에 불 켜라고 델리게이트 보내기
			OnTutorialDashPressed.Broadcast(dashjudge);
		}
		if (AreAllTutorialDashesPressed()) {
			bCanNextStep = true;
		}
	}
}

bool UTutorialManager::AreAllTutorialDashesPressed() const
{
	bool bDashSuccess = PressedDashes.Contains(EJudgementResult::DashSuccess) && PressedDashes[EJudgementResult::DashSuccess];
	bool bDashPerfect = PressedDashes.Contains(EJudgementResult::DashPerfect) && PressedDashes[EJudgementResult::DashPerfect];
	return bDashSuccess && bDashPerfect;
}

void UTutorialManager::OnTutorialAttackInput()
{
	if (currentStep == ETutorialStep::Step7) {
		this->attackCount++;
		OnTutorialAttackPressed.Broadcast(attackCount, attackCountStep8);

		if (attackCount >= 8)
		{
			bCanNextStep = true;
		}
	}
	else if (currentStep == ETutorialStep::Step8) {
		this->attackCountStep8++;
		OnTutorialAttackPressed.Broadcast(attackCount, attackCountStep8);

		if (attackCountStep8 >= 4)
		{
			bCanNextStep = true;
		}
	}
}

void UTutorialManager::OnTutorialHealthUp()
{
	if (currentStep == ETutorialStep::Step9) {
		bCanNextStep = true;
	}
}

void UTutorialManager::OnTutorialEnemySlayed(int SlayedEnemyCount)
{
	if (currentStep == ETutorialStep::Step10) {
		if(SlayedEnemyCount>=2) bCanNextStep = true;
	}
}

void UTutorialManager::OnTutorialConnectEnemy()
{
	if (!EnemyClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundEnemyActors;
	UGameplayStatics::GetAllActorsOfClass(World, EnemyClass, FoundEnemyActors);

	if (FoundEnemyActors.Num() >= 2)
	{
		TutorialEnemy1 = Cast<AEnemyBase>(FoundEnemyActors[0]);
		TutorialEnemy2 = Cast<AEnemyBase>(FoundEnemyActors[1]);

		UE_LOG(LogTemp, Log, TEXT("✅ TutorialEnemy1: %s"), *TutorialEnemy1->GetName());
		UE_LOG(LogTemp, Log, TEXT("✅ TutorialEnemy2: %s"), *TutorialEnemy2->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Enemy 수가 2개 미만입니다. Found: %d"), FoundEnemyActors.Num());
	}

	TArray<AActor*> FoundLaserActors;
	UGameplayStatics::GetAllActorsOfClass(World, LaserClass, FoundLaserActors);

	if (FoundLaserActors.Num() >= 2)
	{
		TutorialLaser1 = Cast<ABeatUnitActor_Laser>(FoundLaserActors[0]);
		TutorialLaser2 = Cast<ABeatUnitActor_Laser>(FoundLaserActors[1]);

		UE_LOG(LogTemp, Log, TEXT("✅ TutorialLaser1: %s"), *TutorialLaser1->GetName());
		UE_LOG(LogTemp, Log, TEXT("✅ TutorialLaser2: %s"), *TutorialLaser2->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Enemy 수가 2개 미만입니다. Found: %d"), FoundLaserActors.Num());
	}

	TArray<AActor*> FoundObstacleActors;
	UGameplayStatics::GetAllActorsOfClass(World, ObstacleClass, FoundObstacleActors);

	if (FoundObstacleActors.Num() >= 1)
	{
		TutorialObstacles = Cast<ABeatUnitActor_Obstacles>(FoundObstacleActors[0]);

		UE_LOG(LogTemp, Log, TEXT("✅ TutorialLaser1: %s"), *TutorialLaser1->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Enemy 수가 1개 미만입니다. Found: %d"), FoundObstacleActors.Num());
	}
}

void UTutorialManager::ActiveUserWidget()
{
	FTutorialUIVisible UIVisiblitys = TutorialDataSet->GetUI_Tmap(currentStep);

	for (uint8 i = 0; i <= static_cast<uint8>(EUIType::State); ++i)
	{
		EUIType type = static_cast<EUIType>(i);

		LOG_KHJ(Error, TEXT("EUItype: %d, Boolen: %s"), i, UIVisiblitys.GetUiActive(type)?TEXT("True"):TEXT("FALSE"))

		OnTutorialUIActive.Broadcast(type, UIVisiblitys.GetUiActive(type));
	}
}
