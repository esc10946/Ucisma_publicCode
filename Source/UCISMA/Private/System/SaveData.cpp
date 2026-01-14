// Fill out your copyright notice in the Description page of Project Settings.


#include "System/SaveData.h"
#include "LevelDesign/GameDataAsset.h"


USaveData::USaveData()
{
	//StageRecord에 공간 선언
	for (int32 i = 0; i < StaticEnum<EStageIndex>()->NumEnums() - 1; i++)
	{
		if (!StageRecord.Contains(static_cast<EStageIndex>(i)))
		{
			StageRecord.Add(static_cast<EStageIndex>(i));
		}
	}
	StageRecord.Add(EStageIndex::AdjustInputOffset);
	//게임 데이터 에셋 경로 탐색
	static ConstructorHelpers::FObjectFinder<UGameDataAsset> GameDataAsset(TEXT("/Game/Data/GameDataAsset.GameDataAsset"));
	if (GameDataAsset.Succeeded())
	{
		GameData = GameDataAsset.Object;
	}
	else
	{
		LOG_DEBUG(Error, TEXT("Fail GameDataAsset Loading"));
	}
}

bool USaveData::UpdateStageRecord(EStageIndex StageIndex, FStageRecord InputRecord)
{

	if (InputRecord.bIsClear)
	{
		StageRecord[StageIndex].bIsClear = true;
	}
	StageRecord[StageIndex].BestScore = FMath::Max(StageRecord[StageIndex].BestScore, InputRecord.BestScore);
	StageRecord[StageIndex].BestClearStrike = FMath::Max(StageRecord[StageIndex].BestClearStrike, InputRecord.BestClearStrike);
	StageRecord[StageIndex].BestPurified = FMath::Max(StageRecord[StageIndex].BestPurified, InputRecord.BestPurified);
	StageRecord[StageIndex].BestCombo = FMath::Max(StageRecord[StageIndex].BestCombo, InputRecord.BestCombo);
	StageRecord[StageIndex].ProgressPercent = FMath::Max(StageRecord[StageIndex].ProgressPercent, InputRecord.ProgressPercent);
	StageRecord[StageIndex].Attempt = FMath::Max(StageRecord[StageIndex].Attempt, InputRecord.Attempt);
	if (InputRecord.Rank == ERankType::S)
	{
		StageRecord[StageIndex].Rank = ERankType::S;
	}
	else if (InputRecord.Rank == ERankType::A && !(StageRecord[StageIndex].Rank == ERankType::S))
	{
		StageRecord[StageIndex].Rank = ERankType::A;
	}
	else if (InputRecord.Rank == ERankType::B&& !(StageRecord[StageIndex].Rank == ERankType::S || StageRecord[StageIndex].Rank == ERankType::A))
	{
		StageRecord[StageIndex].Rank = ERankType::B;
	}
	else if (InputRecord.Rank == ERankType::C && !(StageRecord[StageIndex].Rank == ERankType::S || StageRecord[StageIndex].Rank == ERankType::A || StageRecord[StageIndex].Rank == ERankType::B)) 
	{
		StageRecord[StageIndex].Rank = ERankType::C;
	}

	return true;
}

bool USaveData::SetStageRecord(EStageIndex StageIndex, FStageRecord InputRecord)
{
	StageRecord[StageIndex].bIsClear = InputRecord.bIsClear;
	StageRecord[StageIndex].BestScore = InputRecord.BestScore;
	StageRecord[StageIndex].BestClearStrike = InputRecord.BestClearStrike;
	StageRecord[StageIndex].BestPurified = InputRecord.BestPurified;
	StageRecord[StageIndex].BestCombo = InputRecord.BestCombo;
	StageRecord[StageIndex].ProgressPercent = InputRecord.ProgressPercent;
	StageRecord[StageIndex].Rank = InputRecord.Rank;
	StageRecord[StageIndex].Attempt = InputRecord.Attempt;

	return true;
}

FStageRecord USaveData::LoadStageRecord(EStageIndex StageIndex)
{
	return StageRecord[StageIndex];
}

bool USaveData::ResetStageRecord(EStageIndex StageIndex)
{
	StageRecord[StageIndex].bIsClear = false;
	StageRecord[StageIndex].BestScore = 0;
	StageRecord[StageIndex].BestClearStrike = 0;
	StageRecord[StageIndex].BestPurified = 0;
	StageRecord[StageIndex].BestCombo = 0;
	StageRecord[StageIndex].ProgressPercent = 0;
	StageRecord[StageIndex].Rank = ERankType::None;
	StageRecord[StageIndex].Attempt = 0;
	bisFirst = false;
	return true;
}

bool USaveData::ResetAllStageRecord()
{
	for (int32 i = 0; i < StaticEnum<EStageIndex>()->NumEnums()-1 ; i++)
	{
		ResetStageRecord(static_cast<EStageIndex>(i));
	}

	return true;
}

bool USaveData::SaveInputOffset(float InputOffset)
{
	SettingData.InputOffset = InputOffset;

	return true;
}

bool USaveData::SaveBgmVolume(int BgmVolume)
{
	SettingData.BgmVolume = BgmVolume;

	return true;
}

bool USaveData::SaveSfxVolume(int SfxVolume)
{
	SettingData.SfxVolume = SfxVolume;

	return true;
}

bool USaveData::SaveUIVolume(int UIVolume)
{
	SettingData.UIVolume = UIVolume;

	return true;
}

bool USaveData::SaveMetronomeVolume(int Volume)
{
	SettingData.MetronomeVolume = Volume;

	return true;
}

bool USaveData::SaveGraphicQuality(int Quality)
{
	SettingData.GraphicQuality = Quality;

	return true;
}

FSettingData USaveData::LoadSettingData()
{
	return SettingData;
}

FSettingData USaveData::ResetSettingData()
{
	SettingData = GameData->GetDefaultSettingData();
	
	return SettingData;
}

void USaveData::doBanchmark() {
	bisBenchmark = true;
}

