// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/UnitManager.h"
#include "Manager/StageManager.h"
#include "BeatUnit/BeatUnitInterface.h"

// Sets default values
AUnitManager::AUnitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BeatUnitSpawnIndex = 0;
}

// Called when the game starts or when spawned
void AUnitManager::BeginPlay()
{
	Super::BeginPlay();
	LOG_PSJ(Warning, TEXT("New Game Start-----------------------"));
	
}

void AUnitManager::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
	BeatUnitSpawnIndex = 0;
	//StageManager->BeatUpdateListeners.AddDynamic(this, &UWidget_BeatBar::BeatUpdate);

	//비트 유닛과 비트매니저 연결 및 비트 유닛 초기화
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BeatUnit"), BeatUnits);
	//BeatIndex와 QuaterBeatIndex를 기준으로 BeatUnits를 오름차순 정렬하여 유닛 소환을 원활하게 세팅
	BeatUnits.Sort([](AActor& A, AActor& B) {
		IBeatUnitInterface* BeatUnitA = Cast<IBeatUnitInterface>(&A);
		IBeatUnitInterface* BeatUnitB = Cast<IBeatUnitInterface>(&B);
		if (!BeatUnitA || !BeatUnitB)
		{
			LOG_DEBUG(Fatal, TEXT("BeatUnits include Actors that don't have BeatUnitInterface"));
		}
		return 4 * BeatUnitA->GetBeatIndex() + BeatUnitA->GetQuaterBeatIndex() < 4 * BeatUnitB->GetBeatIndex() + BeatUnitB->GetQuaterBeatIndex();
		});

	for (AActor* BeatUnit : BeatUnits)
	{
		IBeatUnitInterface* BeatUnitInterface = Cast<IBeatUnitInterface>(BeatUnit);
		BeatUnitInterface->DeactiveUnit();
	}
	StageManager->BeatUpdateListeners.AddDynamic(this, &AUnitManager::BeatUpdate);
}

void AUnitManager::BeatUpdate(int QuaterBeatIndex)
{
	//BeatUnitSpawnIndex가 BeatUnits의 크기보다 크거나 같으면 Out of Range가 발생한다.
	if (BeatUnitSpawnIndex < BeatUnits.Num())
	{
		IBeatUnitInterface* BeatUnit = Cast<IBeatUnitInterface>(BeatUnits[BeatUnitSpawnIndex]);

		while (BeatUnit && (IsUnitPastBeat(BeatUnit, QuaterBeatIndex) || IsUnitCurrentBeat(BeatUnit, QuaterBeatIndex)))
		{
			BeatUnit->ActiveUnit();
			BeatUnitSpawnIndex++;
			if(BeatUnitSpawnIndex >= BeatUnits.Num())
			{
				break;
			}
			BeatUnit = Cast<IBeatUnitInterface>(BeatUnits[BeatUnitSpawnIndex]);
		}
	}
	
}

bool AUnitManager::IsUnitPastBeat(IBeatUnitInterface* BeatUnit, int QuaterBeatIndex)
{
	return BeatUnit->GetBeatIndex() < StageManager->GetBeatIndex() || (BeatUnit->GetBeatIndex() == StageManager->GetBeatIndex() && BeatUnit->GetQuaterBeatIndex() < QuaterBeatIndex);
}

bool AUnitManager::IsUnitCurrentBeat(IBeatUnitInterface* BeatUnit, int QuaterBeatIndex)
{
	return BeatUnit->GetBeatIndex() == StageManager->GetBeatIndex() && BeatUnit->GetQuaterBeatIndex() == QuaterBeatIndex;
}


