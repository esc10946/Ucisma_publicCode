// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"
#include "Manager/UIManager.h"
#include "Manager/UnitManager.h"
#include "Manager/SoundManager.h"
#include "Manager/TutorialManager.h"
#include "Core/MyGameInstance.h"
#include "Widget/Widget_Score.h"
#include "Kismet/GameplayStatics.h"
#include "LevelDesign/GameDataAsset.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Application/SlateApplication.h"
#include "System/SaveData.h"
#include "PlayerCharacter.h"
#include "LevelDesign/Camera_Map.h"
#include "BeatUnit/Enemy/BeatUnitCharacter_Boss_Harmyth.h"

#define SECONDS_PER_MINUTE 60.0f
#define INF 9999

// Sets default values
AStageManager::AStageManager()
{
	// 변수 초기화
	InitVariables();
	//게임 데이터 에셋에서 데이터를 가져온다.
	LoadGameDataAsset();
}

void AStageManager::InitVariables()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PurificationSuccessCount = 0;
	PurificationFailureCount = 0;
	DelayForSongAndBeat = 0.5f;
	DebugModeToggles.Emplace(EDebugMode::DebugMode, false);
	DebugModeToggles.Emplace(EDebugMode::AllPerfect, false);
	DebugModeToggles.Emplace(EDebugMode::InfinityPurification, false);
	DebugModeToggles.Emplace(EDebugMode::InfinityHealthAndStamina, false);
	DebugModeToggles.Emplace(EDebugMode::Invincibility, false);
	DebugModeToggles.Emplace(EDebugMode::FixPlayer, false);
	DebugModeToggles.Emplace(EDebugMode::InfinityDamage, false);
	DebugModeToggles.Emplace(EDebugMode::ViewBeatIndex, false);
	DebugModeToggles.Emplace(EDebugMode::InfinityRange, false);
	GameSpeed = 1.0f;
	ActionDelayQuaterBeat.Emplace(EJudgementAction::Attack, 0);
	ActionDelayQuaterBeat.Emplace(EJudgementAction::Dash, 0);
	TutorialManager = FindComponentByClass<UTutorialManager>();
}

bool AStageManager::GetCameraLocationAtBeatIndex(int BeatIndex, FVector& OutLocation)
{
	// 유효성 검사
	if (!MapArray)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tag_Map"), FoundActors);
		if (FoundActors.Num() > 0)
		{
			MapArray = Cast<AMap_Array_Actor>(FoundActors[0]);
		}

		if (!MapArray)
		{
			LOG_KHJ(Error, TEXT("MapArray is null."));
			return false;
		}
	}

	const TArray<FVector>& Positions = MapArray->Tile_Position;
	const TArray<float>& Speeds = MapArray->Tile_Speed_Index;
	const TArray<FTileSkipInfo>& Skips = MapArray->Tile_Skip_Index;
	const TMap<int32, int32>& BeatSkips = MapArray->Beat_Skip_Index;

	if (Positions.Num() < 2 || Speeds.Num() < 1 || Skips.Num() < 1)
	{
		LOG_KHJ(Error, TEXT("Insufficient map data."));
		return false;
	}

	float BeatTime = BeatIndex * SecondPerBeat;
	float AccumulatedBeats = 0.f;

	int32 BeforeIndex = 0;
	int32 CurrentIndex = 1;

	// ✅ 1. BeatSkip 바로 처리 (BeatIndex 기준)
	if (BeatSkips.Contains(BeatIndex))
	{
		int32 SkipTileIndex = BeatSkips[BeatIndex];
		if (Positions.IsValidIndex(SkipTileIndex))
		{
			OutLocation = Positions[SkipTileIndex];
			LOG_KHJ(Display, TEXT("BeatSkip: Beat %d → Tile %d → %s"),
				BeatIndex, SkipTileIndex, *OutLocation.ToString());
			return true;
		}
		else
		{
			LOG_KHJ(Error, TEXT("Invalid SkipTileIndex: %d"), SkipTileIndex);
			return false;
		}
	}

	// ✅ 2. 일반 타일 보간
	while (CurrentIndex < Positions.Num())
	{
		if (!Speeds.IsValidIndex(BeforeIndex) || !Skips.IsValidIndex(BeforeIndex))
		{
			LOG_KHJ(Error, TEXT("Invalid data at index %d"), BeforeIndex);
			return false;
		}

		bool bSkip = Skips[CurrentIndex].bSkipTile;

		if (bSkip)
		{
			BeforeIndex = ++CurrentIndex;
			CurrentIndex++;

			continue;
		}

		float TileSpeed = Speeds[CurrentIndex];
		float MovePerBeat = (BeatPerSecond / 4.f) * Camera_Speed * TileSpeed;
		float BeatsToTraverseTile = 0.f;

		if (MovePerBeat > 0.f)
		{
			BeatsToTraverseTile = 1.0f / MovePerBeat;
		}
		else
		{
			// ✅ Speed가 0인 경우: 위치는 그대로, 시간은 흐른다
			BeatsToTraverseTile = 1/(BeatPerSecond / 4.f); // 또는 원하는 정지 길이 비트 수
		}

		if (AccumulatedBeats + BeatsToTraverseTile >= BeatTime)
		{
			float IntoTileBeat = BeatTime - AccumulatedBeats;
			float Percent = MovePerBeat > 0.f ? (IntoTileBeat / BeatsToTraverseTile) : 0.0f;

			if (!Positions.IsValidIndex(BeforeIndex))
			{
				LOG_KHJ(Error, TEXT("Invalid Position Index: %d"), BeforeIndex);
				return false;
			}

			// ✅ 위치는 고정 (정지 상태)
			if (MovePerBeat <= 0.f)
			{
				OutLocation = Positions[BeforeIndex];
			}
			else
			{
				if (!Positions.IsValidIndex(CurrentIndex))
				{
					LOG_KHJ(Error, TEXT("Invalid Lerp Index: %d"), CurrentIndex);
					return false;
				}
				OutLocation = FMath::Lerp(Positions[BeforeIndex], Positions[CurrentIndex], Percent);
			}

			LOG_KHJ(Display, TEXT("Beat %.2f → Tile %d%s → %s"),
				BeatTime, BeforeIndex,
				MovePerBeat <= 0.f ? TEXT(" (Stopped)") : *FString::Printf(TEXT(" to %d (%.2f%%)"), CurrentIndex, Percent * 100.f),
				*OutLocation.ToString());

			return true;
		}

		AccumulatedBeats += BeatsToTraverseTile;
		BeforeIndex = CurrentIndex++;
	}

	OutLocation = Positions.Last();
	LOG_KHJ(Warning, TEXT("Beat %.2f exceeds timeline (%.2f total)"), BeatTime, AccumulatedBeats);
	return false;
}

// Called when the game starts or when spawned
void AStageManager::BeginPlay()
{
	Super::BeginPlay();

	InitStage();
}

// Called every frame
void AStageManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DebugModeToggles[EDebugMode::FixPlayer])
	{
		Camera->Setup_Player_Location();
	}
	if (IsLoadingComplete() && !bIsStartStage)
	{
		//스테이지 시작을 위한 비트 및 노래 시작
		GameInstance->RemoveLoadingUI();
		StartBeatAndSong();
		bIsStartStage = true;
		//SetActorTickEnabled(false);
	}
}

void AStageManager::InitStage()
{
	//게임 데이터 에셋에서 필요한 데이터를 가져옴
	LoadGameDataAsset();
	//스테이지의 초기 설정
	SetupStageBase();
	//특정 스테이지에만 적용되는 설정
	SetupStageSpecial();
	//매니저를 생성하고 초기화
	InitManagers();
	//중간에서 시작할 시 관련 설정
	SetupStageSkip();
	bIsInitStageComplete = true;
}

void AStageManager::SetupStageBase()
{
	//게임 진행 속도 설정
	GetWorldSettings()->SetTimeDilation(GameSpeed);

	//포커스 인, 포커스 아웃시 호출할 함수 설정
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AStageManager::OnFocusChange);

	//플레이어가 설정한 InputOffset 적용
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	InputOffset = GameInstance->InputOffset;

	//세이브 슬롯 이름을 게임 인스턴스에서 가져옴
	SaveSlotName = Cast<UMyGameInstance>(GetGameInstance())->SaveSlotName;

	//플레이어를 레벨에서 찾음
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//카메라를 레벨에서 찾음
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tag_Camera"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		Camera = Cast<ACamera_Map>(FoundActors[0]);
	}

	//해당 스테이지의 attempt를 1 늘림
	USaveData* SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	FStageRecord AttemptRecord;
	AttemptRecord.Attempt = SaveData->LoadStageRecord(StageIndex).Attempt + 1;
	SaveData->UpdateStageRecord(StageIndex, AttemptRecord);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);


}

void AStageManager::SetupStageSpecial()
{
	//싱크 조절 맵에서만 적용되는 설정
	if (StageIndex == EStageIndex::AdjustInputOffset)
	{
		InputOffset = 0;
	}

	//튜토리얼 맵에서만 적용 되는 설정
	if (IsTutorialStage())
	{
		//뭔가를 해 ㅇㅇ
		TutorialManager = FindComponentByClass<UTutorialManager>();
		if (TutorialManager) {
			TutorialManager->Init(this);
			UE_LOG(LogTemp, Display, TEXT("TutorialManager Init"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TutorialManager is null!"));
		}
	}

	if (IsBossStage())
	{
		TArray<AActor*> FoundActors;

		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Boss"), FoundActors);
		Boss = Cast<ABeatUnitCharacter_Boss_Harmyth>(FoundActors[0]);
	}

}

void AStageManager::InitManagers()
{
	BeatManager = GetWorld()->SpawnActor<ABeatManager>(GetActorLocation(), GetActorRotation());
	UIManager = GetWorld()->SpawnActor<AUIManager>(GetActorLocation(), GetActorRotation());
	UnitManager = GetWorld()->SpawnActor<AUnitManager>(GetActorLocation(), GetActorRotation());
	SoundManager = GetWorld()->SpawnActor<ASoundManager>(GetActorLocation(), GetActorRotation());

	if (BeatManager)
	{
		BeatManager->Init(this);
	}
	if (UIManager)
	{
		UIManager->Init(this);
	}
	if (UnitManager)
	{
		UnitManager->Init(this);
	}
	if (SoundManager)
	{
		SoundManager->Init(this);
	}
}

void AStageManager::SetupStageSkip()
{
	if (!Camera || StageStartIndex <= 0)
	{
		//LOG_KHJ(Error, TEXT("StageStartIndex == 0!"));
		return;
	}

	//카메라를 시작 타일로 옮김
	Camera->FindMap();
	if (Camera->Tile_Position.Num() > 1)
	{
		StageStartIndex = FMath::Min(StageStartIndex, Camera->Tile_Position.Num() - 1);
		StageStartBeatIndex = Camera->TileIndexToBeatIndex(StageStartIndex);
		//만약 스킵되는 타일을 시작 타일로 했을 경우 가장 가까운 일반 타일로 이동
		while (StageStartBeatIndex == -1)
		{
			StageStartBeatIndex = Camera->TileIndexToBeatIndex(++StageStartIndex);;
		}
		BeatManager->SetBeatIndex(StageStartBeatIndex);
		//카메라의 인덱스들을 지정하여 카메라 위치 조정
		Camera->Before_Index = StageStartIndex;
		Camera->Current_Index = StageStartIndex + 1;
	}
	Camera->SetActorLocation(Camera->Tile_Position[StageStartIndex]);


	//노래를 시작 비트부터 시작하게 바꿈
	SoundManager->SetSongStartTime(GetBeatIndex() * GetSecondPerBeat());

	//보스가 있고 StartBeatIndex보다 낮은 BeatIndex에서 소환되는 경우 보스가 플레이어를 올바르게 추적하도록 좌표를 계산하여 반영
	if (Boss && Boss->GetBeatIndex() <= StageStartBeatIndex)
	{
		//// 보스 시작위치와 첫 타일 위치만큼 움직여서 보스가 화면 내에 위치하도록 만든다.
		//int BossStartTileIndex = Camera->BeatIndexToTileIndex(Boss->GetBeatIndex());
		//FVector BossStartTileLocation = Camera->Tile_Position[BossStartTileIndex];
		//FVector StartTileLocation = Camera->Tile_Position[0];
		//FVector LocationDifference = (StartTileLocation - BossStartTileLocation);
		//Boss->SetActorLocation(Boss->GetActorLocation() + LocationDifference);
		// 보스의 BossIndex를 현재 BeatIndex에 맞추어 바꾼다.
		Boss->SetBossIndex(StageStartBeatIndex);
		Boss->BossSetupStageSkip(StageStartBeatIndex);
	}
}


bool AStageManager::SaveStageRecord(bool bIsComplete)
{
	USaveData* SaveInstance;
	//이미 세이브 파일이 존재하는 경우
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveInstance = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	//이미 세이브 파일이 존재하지 않는 경우
	else
	{
		SaveInstance = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}
	 
	if (!SaveInstance)
	{
		LOG_DEBUG(Error, TEXT("Don't Exist Save File"));
		return false;
	}

	FStageRecord StageRecord;

	StageRecord.bIsClear = bIsComplete;
	StageRecord.BestClearStrike = ClearStrikeCount;
	StageRecord.BestCombo = MaxCombo;
	StageRecord.BestPurified = PurificationSuccessCount;
	StageRecord.BestScore = Score;
	StageRecord.ProgressPercent = static_cast<float>(GetBeatIndex()) / GetLastBeatIndex() * 100;
		
	//랭크의 기준이 되는 점수를 가져옴
	BaseScore = GetBaseScore(StageIndex);
	//랭크의 기준이 되는 적의 수를 가져옴
	EnemyCount = GetEnemyCount();

	/*bool bIsAllEnemiesPurified = PurificationSuccessCount == EnemyCount;
	bool bIsAllClearStrike = ClearStrikeCount >= PurificationSuccessCount;
	bool bIsOverBaseScore = Score >= BaseScore;*/

	bool bOverScore_S = Score >= BaseScore * 0.9f;
	bool bOverScore_A = Score >= BaseScore * 0.75f;
	bool bOverScore_B = Score >= BaseScore * 0.5f;

	if (!bIsComplete)
	{
		StageRecord.Rank = ERankType::None;
	}
	else if (!bOverScore_B)
	{
		StageRecord.Rank = ERankType::C;
	}
	else if (!bOverScore_A)
	{
		StageRecord.Rank = ERankType::B;
	}
	else if (!bOverScore_S)
	{
		StageRecord.Rank = ERankType::A;
	}
	else
	{
		StageRecord.Rank = ERankType::S;
	}
	SaveInstance->UpdateStageRecord(StageIndex, StageRecord);
	UGameplayStatics::SaveGameToSlot(SaveInstance, SaveSlotName, 0);

	return true;
}


bool AStageManager::IsBossStage() const
{
	return (StageIndex == EStageIndex::Stage_1_3 ||
			StageIndex == EStageIndex::Stage_2_3 ||
			StageIndex == EStageIndex::Stage_3_3 ||
			StageIndex == EStageIndex::Stage_4_3);
}

void AStageManager::BossDeadHandler()
{
	PlaySfx(ESfx::BossDied);
	Player->ActivateInput(false);
}

void AStageManager::LoadGameDataAsset()
{
	static ConstructorHelpers::FObjectFinder<UGameDataAsset> DataAsset(TEXT("/Game/Data/GameDataAsset.GameDataAsset"));

	if (!DataAsset.Succeeded())
	{
		LOG_DEBUG(Warning, TEXT("GameDataAsset Loading Failed"));
	}

	GameDataAsset = DataAsset.Object;
	const FStageData StageData = GameDataAsset->GetStageData(StageIndex);
	Song = StageData.Song;
	Bpm = StageData.Bpm;
	SongLength = StageData.SongLength;
	MaxPurificationFailureCount = StageData.MaxPurificationFailureCount;
	PerfectPercentageOfAttack = GameDataAsset->GetPerfectPercentageOfAttack();
	GreatPercentageOfAttack = GameDataAsset->GetGreatPercentageOfAttack();
	GoodPercentageOfAttack = GameDataAsset->GetGoodPercentageOfAttack();
	SuccessPercentageOfDash = GameDataAsset->GetSuccessPercentageOfDash();
	PerfectPercentageOfDash = GameDataAsset->GetPerfectPercentageOfDash();
	SecondPerBeat = SECONDS_PER_MINUTE / static_cast<float>(Bpm);
	BeatPerSecond = static_cast<float>(Bpm) / SECONDS_PER_MINUTE;
	Player_Range = GameDataAsset->GetPlayer_Attack_Range();
	Camera_Speed = StageData.CameraSpeed;
	Player_Dash_Range = GameDataAsset->GetPlayer_Dash_Range();
	bIsPurification = StageData.bIsPurifyActive;
}

void AStageManager::StartBeatAndSong()
{
	FTimerHandle Temp1;
	FTimerHandle Temp2;
	GetWorld()->GetTimerManager().SetTimer(Temp1, BeatManager, &ABeatManager::StartBeat, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(Temp2, SoundManager, &ASoundManager::StartSong, 1.0f + DelayForSongAndBeat);
}

bool AStageManager::IsLoadingComplete()
{
	return bIsInitStageComplete && GameInstance->IsAssetLoadingComplete();
}

void AStageManager::SetStagePaused(bool bPaused)
{
	UGameplayStatics::SetGamePaused(this, bPaused);
	bIsGamePaused = bPaused;
	if(bPaused)
	{
		SoundManager->PauseSong();
	}
	else
	{
		SoundManager->ResumeSong();
	}
}

void AStageManager::GameOver(EGameOverType GameOverType)
{
	SetStagePaused(true);
	BeatManager->GetWorldTimerManager().ClearTimer(BeatManager->BeatTimerHandle);
	UIManager->ShowGameOverUI(GameOverType);
	if (GameOverType == EGameOverType::GameClear_Boss || GameOverType == EGameOverType::GameClear_Normal)
	{
		SaveStageRecord(true);
	}
	else
	{
		SaveStageRecord(false);
	}
}

EJudgementResult AStageManager::CheckJudgement(EJudgementAction Action)
{
	EJudgementResult result = BeatManager->CheckJudgement(Action);
	UIManager->DeleteNote(Action);
	UIManager->ShowJudgementHighlight(result);

	return result;
}

void AStageManager::ShowPurificationFailureCount(FVector2D ActorScreenPos)
{
	UIManager->ShowPurificationFailureCount(ActorScreenPos);
}

void AStageManager::FailPurify(FVector2D ActorScreenPos)
{
	if (bIsPurification) {
		SetPurificationFailureCount(PurificationFailureCount + 1);
		ShowPurificationFailureCount(ActorScreenPos);
		if (MaxPurificationFailureCount - PurificationFailureCount <= 3)
		{
			SoundManager->PlaySfx(ESfx::PurificationFailDanger);
		}
		else
		{
			SoundManager->PlaySfx(ESfx::PurificationFailNormal);
		}
		if (PurificationFailureCount >= MaxPurificationFailureCount)
		{
			GameOver(EGameOverType::GameFailed_PurificationFailed);
		}
	}
}

void AStageManager::PlaySfx(ESfx SfxType)
{
	if (SoundManager)
	{
		SoundManager->PlaySfx(SfxType);
	}
}

void AStageManager::OnFocusChange(bool bIsActive)
{
	if (bIsActive)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tag_Camera"), FoundActors);

		if (FoundActors.Num() > 0)
		{
			// 태그를 가진 첫 번째 카메라를 가져옵니다
			AActor* CameraActor = FoundActors[0];
			ACamera_Map* CamActor = Cast<ACamera_Map>(FoundActors[0]);
			if (CamActor)
			{
				CamActor->Setup_Player_Location();
			}
		}
		//플레이어 위치 재정정하는 함수
	}
	else if(bIsGamePaused == false)
	{
		SetStagePaused(true);
		UIManager->ShowPause();
	}
}

void AStageManager::IncreasePurificationCount(int value)
{
	SetPurificationSuccessCount(GetPurificationSuccessCount() + value);
	if (IsTutorialStage()) {
		TutorialManager->OnTutorialEnemySlayed(GetPurificationSuccessCount());
	}
	else if (IsBossStage()){
		if (OnPurificationSuccess.IsBound()) OnPurificationSuccess.Broadcast(); // 🔹 보스에게 정화 성공 알림
	}
}

void AStageManager::PrintPlayerOutMessage()
{
	if (UIManager)
	{
		UIManager->PrintPlayerOutMessage();
	}
}

void AStageManager::ToggleDebugMode()
{
	DebugModeToggles[EDebugMode::DebugMode] = !DebugModeToggles[EDebugMode::DebugMode];
	ActivateDebugMode(DebugModeToggles[EDebugMode::DebugMode]);
}

void AStageManager::ToggleAllPerfectMode()
{
	DebugModeToggles[EDebugMode::AllPerfect] = !DebugModeToggles[EDebugMode::AllPerfect];
	ActivateAllPerfectMode(DebugModeToggles[EDebugMode::AllPerfect]);
}

void AStageManager::ToggleInfinityPurificationMode()
{
	DebugModeToggles[EDebugMode::InfinityPurification] = !DebugModeToggles[EDebugMode::InfinityPurification];
	ActivateInfinityPurificationMode(DebugModeToggles[EDebugMode::InfinityPurification]);
}

void AStageManager::ToggleInfinityHealthAndStaminaMode()
{
	DebugModeToggles[EDebugMode::InfinityHealthAndStamina] = !DebugModeToggles[EDebugMode::InfinityHealthAndStamina];
	ActivateInfinityHealthAndStaminaMode(DebugModeToggles[EDebugMode::InfinityHealthAndStamina]);
}

void AStageManager::ToggleInvincibilityMode()
{
	DebugModeToggles[EDebugMode::Invincibility] = !DebugModeToggles[EDebugMode::Invincibility];
	ActivateInvincibilityMode(DebugModeToggles[EDebugMode::Invincibility]);
}

void AStageManager::ToggleFixPlayerMode()
{
	DebugModeToggles[EDebugMode::FixPlayer] = !DebugModeToggles[EDebugMode::FixPlayer];
	ActivateFixPlayerMode(DebugModeToggles[EDebugMode::FixPlayer]);
}

void AStageManager::ToggleInfinityDamage()
{
	DebugModeToggles[EDebugMode::InfinityDamage] = !DebugModeToggles[EDebugMode::InfinityDamage];
	ActivateInfinityDamage(DebugModeToggles[EDebugMode::InfinityDamage]);
}

void AStageManager::ToggleViewBeatIndex()
{
	DebugModeToggles[EDebugMode::ViewBeatIndex] = !DebugModeToggles[EDebugMode::ViewBeatIndex];
	//ActivateInfinityPurificationMode(DebugModeToggles[EDebugMode::ViewBeatIndex]);
	ActivateViewBeatIndexMode(DebugModeToggles[EDebugMode::ViewBeatIndex]);
}

void AStageManager::ToggleInfinityRange()
{
	DebugModeToggles[EDebugMode::InfinityRange] = !DebugModeToggles[EDebugMode::InfinityRange];
	ActivateInfinityRange(DebugModeToggles[EDebugMode::InfinityRange]);
}

void AStageManager::ActivateDebugMode(bool bActive)
{
	UIManager->ToggleDebugMode(bActive);
}

void AStageManager::ActivateAllPerfectMode(bool bActive)
{
	UIManager->UpdateDebugMode();
	if (bActive)
	{
		BeatManager->SetPerfectPercentageOfAttack(1.0f);
		BeatManager->SetPerfectPercentageOfDash(1.0f);
	}
	else
	{
		BeatManager->SetPerfectPercentageOfAttack(PerfectPercentageOfAttack);
		BeatManager->SetPerfectPercentageOfDash(PerfectPercentageOfDash);
	}
}

void AStageManager::ActivateInfinityPurificationMode(bool bActive)
{
	static int OriginValue = PurificationFailureCount;

	UIManager->UpdateDebugMode();
	if (bActive)
	{
		OriginValue = PurificationFailureCount;
		PurificationFailureCount = -INF;
	}
	else
	{
		PurificationFailureCount = OriginValue;
	}
}

void AStageManager::ActivateInfinityHealthAndStaminaMode(bool bActive)
{
	static int originValue = Player->Player_MAX_HP;

	UIManager->UpdateDebugMode();
	if (bActive)
	{
		for (int i = 0; i < Player->Player_MAX_HP; i++)
		{
			Player->Take_HP(1, this);
		}
		originValue = Player->Player_MAX_HP;
		Player->Player_MAX_HP = INF;
		Player->Player_Current_HP = INF;
		Player->Player_Current_Stamina = INF;
	}
	else
	{
		Player->Player_MAX_HP = originValue;
		Player->Player_Current_HP = Player->Player_MAX_HP;
		Player->Player_Current_Stamina = Player->Player_MAX_Stamina;
	}
}

void AStageManager::ActivateInvincibilityMode(bool bActive)
{
	UIManager->UpdateDebugMode();
	Player->bIsInvincibility = bActive;
}

void AStageManager::ActivateFixPlayerMode(bool bActive)
{
	UIManager->UpdateDebugMode();
	if (bActive)
	{
		Camera->Setup_Player_Location();
		Player->AttachToCamera();
		if (!DebugModeToggles[EDebugMode::Invincibility])
		{
			ToggleInvincibilityMode();
		}
		if (!DebugModeToggles[EDebugMode::InfinityPurification])
		{
			ToggleInfinityPurificationMode();
		}
	}
	else
	{
		Player->DetachFromCamera();
	}
}

void AStageManager::ActivateInfinityDamage(bool bActive)
{
	static float OriginDamageValue = Player->Player_Attack_Damage_Base;;
	UIManager->UpdateDebugMode();
	if (bActive)
	{
		OriginDamageValue = Player->Player_Attack_Damage_Base;
		Player->Player_Attack_Damage_Base = INF;
	}
	else
	{
		Player->Player_Attack_Damage_Base = OriginDamageValue;
	}
}

void AStageManager::ActivateInfinityRange(bool bActive)
{
	static float OriginAttackRange = Player->GetPlayerAttackRange();
	static FVector OriginSwordScale = Player->Sword_Mesh->GetRelativeScale3D();
	static float OriginScaleInDash = Player->scale_indash;
	UIManager->UpdateDebugMode();
	if (bActive)
	{
		OriginAttackRange = Player->GetPlayerAttackRange();
		Player->SetPlayerScale(OriginAttackRange * 10, Player->Dash_Power);
		OriginSwordScale = Player->Sword_Mesh->GetRelativeScale3D();
		Player->Sword_Mesh->SetRelativeScale3D(OriginSwordScale + (FVector::OneVector * 100));
		OriginScaleInDash = Player->scale_indash;
		Player->scale_indash *= 10.0f;
	}
	else
	{
		Player->SetPlayerScale(OriginAttackRange, Player->Dash_Power);
		Player->Sword_Mesh->SetRelativeScale3D(OriginSwordScale);
		Player->scale_indash = OriginScaleInDash;
	}
}

void AStageManager::ShowPauseUI()
{
	UIManager->ShowPause();
}

void AStageManager::AddScore(int Value)
{
	Score += Value;

	if (UIManager->GetScoreInstance())
	{
		UIManager->GetScoreInstance()->UpdateScore(Score);
	}
}

void AStageManager::PlayAddScoreAnim(int Value)
{
	if (UIManager->GetScoreInstance())
	{
		UIManager->GetScoreInstance()->PlayScoreIncreaseAnim(Value);
	}
}

void AStageManager::AddClearStrikeCount(int Value)
{
	ClearStrikeCount += Value;
}

void AStageManager::AddCombo(int Value)
{
	SetNowCombo(NowCombo + Value);
	UIManager->UpdateCombo(NowCombo);
	
}

int AStageManager::GetBeatIndex() {	return BeatManager->GetBeatIndex(); }
int AStageManager::GetQuaterBeatIndex() { return BeatManager->GetQuaterBeatIndex(); }
float AStageManager::GetSecondPerBeat() { return SecondPerBeat; }
float AStageManager::GetBeatPerSecond() { return BeatPerSecond; }
int AStageManager::GetLastBeatIndex() { return BeatManager->GetLastBeatIndex(); }

//기준점수 구하는 함수
float AStageManager::GetBaseScore(EStageIndex stageindex) {
	
	//Bpm // A: 스테이지 BPM
	//SongLength  // B: 스테이지 총 길이(초)

	//float stagetime = SongLength / 60.0f;  // C: 스테이지 길이(분)
	//float beatcount = stagetime * Bpm; // D: 총 비트 수

	//int e, f;
	//if (beatcount == static_cast<int>(beatcount)) {
	//	e = static_cast<int>(beatcount) - 1;
	//	f = static_cast<int>(beatcount);
	//}
	//else {
	//	e = static_cast<int>(beatcount);
	//	f = static_cast<int>(beatcount) + 1;
	//}

	//// G: 최종 기준 점수
	//int g = 100 + (10 * f) + (500 * (e / 8));
	if (stageindex == EStageIndex::Stage_1_1)
	{
		return 16740;
	}
	if (stageindex == EStageIndex::Stage_1_2)
	{
		return 83835;
	}
	if (stageindex == EStageIndex::Stage_1_3)
	{
		return 72650;
	}

	return -1;
}

bool AStageManager::IsTutorialStage() const
{
	return StageIndex == EStageIndex::StageTutorial;
}

int AStageManager::GetEnemyCount() {
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PurificationTarget"), FoundActors);
	return FoundActors.Num();
}

int AStageManager::GetPercentageBetween(float Min, float Max, float Value)
{
	if (FMath::IsNearlyEqual(Max, Min)) return 0.0f; // divide by zero 방지
	return (Value - Min) / (Max - Min) * 100.0f;
}

FVector AStageManager::GetPositionBetweenByPercent(const FVector& A, const FVector& B, float Percent)
{
	float Alpha = FMath::Clamp(Percent / 100.0f, 0.0f, 1.0f);
	return FMath::Lerp(A, B, Alpha);
}

void AStageManager::SetNowCombo(int Value)
{
	NowCombo = Value;
	SetMaxCombo(FMath::Max(MaxCombo, NowCombo));
}

void AStageManager::ActivatePlayerInputIgnore(bool bActivate)
{
	if (Player)
	{
		Player->ActivateInput(!bActivate);
	}
}

void AStageManager::ActivateViewBeatIndexMode(bool bActive)
{
	BeatManager->SetViewDebugMode(bActive);
	UIManager->UpdateDebugMode();
}
