// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesign/GameDataAsset.h"

const FStageData UGameDataAsset::GetStageData(EStageIndex StageIndex)
{
	FStageData ReturnData;
	if (!StageData.Find(StageIndex))
	{
		return ReturnData;
	}
	ReturnData = *StageData.Find(StageIndex);
	
	return ReturnData;
}

