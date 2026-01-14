// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/SaveGame.h"
#include "System/CommonTypes.h"
#include "SaveData.generated.h"





UCLASS()
class UCISMA_API USaveData : public USaveGame
{
	GENERATED_BODY()
public:
	USaveData();
	//지정한 스테이지의 기록을 업데이트한다. 세이브한 데이터와 인자 값을 비교하여 더 적절한 데이터를 저장한다.
	UFUNCTION(BlueprintCallable)
	bool UpdateStageRecord(EStageIndex StageIndex, FStageRecord InputRecord);
	//지정한 스테이지의 기록을 저장한다. 기존 데이터와 상관 없이 무조건 인자 값으로 저장된다.
	UFUNCTION(BlueprintCallable)
	bool SetStageRecord(EStageIndex StageIndex, FStageRecord InputRecord);
	//지정한 스테이지의 기록을 가져온다. FStageRecord 형태로 반환된다.
	UFUNCTION(BlueprintCallable)
	FStageRecord LoadStageRecord(EStageIndex StageIndex);
	//지정한 스테이지의 기록을 삭제한다.
	UFUNCTION(BlueprintCallable)
	bool ResetStageRecord(EStageIndex StageIndex);
	//모든 스테이지의 기록을 삭제한다.
	UFUNCTION(BlueprintCallable)
	bool ResetAllStageRecord();
	//Setting의 InputOffset을 저장한다.
	UFUNCTION(BlueprintCallable)
	bool SaveInputOffset(float InputOffset);
	//Setting의 BgmVolume을 저장한다.
	UFUNCTION(BlueprintCallable)
	bool SaveBgmVolume(int BgmVolume);
	//Setting의 SfxVolume을 저장한다.
	UFUNCTION(BlueprintCallable)
	bool SaveSfxVolume(int SfxVolume);
	//Setting의 UIVolume을 저장한다.
	UFUNCTION(BlueprintCallable)
	bool SaveUIVolume(int UIVolume);
	UFUNCTION(BlueprintCallable)
	bool SaveMetronomeVolume(int Volume);

	//Setting의 GraphicQuality를 저장한다.
	UFUNCTION(BlueprintCallable)
	bool SaveGraphicQuality(int Quality);

	//세팅 데이터를 불러온다.
	UFUNCTION(BlueprintCallable)
	FSettingData LoadSettingData();
	//세팅 데이터를 리셋한다. 기본값으로 초기화하고 그 값을 반환한다.
	UFUNCTION(BlueprintCallable)
	FSettingData ResetSettingData();
	//벤치마킹 플립 함수
	UFUNCTION(BlueprintCallable)
	void doBanchmark();
	UFUNCTION(BlueprintCallable)
	bool GetisBenchmark() { return bisBenchmark; }

	UFUNCTION(BlueprintCallable)
	void doFirst() {
		bisFirst = true;
	}


	UFUNCTION(BlueprintCallable)
	bool GetisFirst() { return bisFirst; }
private:
	//초반 초기화를 위한 bool 함수
	UPROPERTY(VisibleAnywhere)
	bool bIsInit;

	//벤치마킹 실행여부(최초1회)
	UPROPERTY(VisibleAnywhere)
	bool bisBenchmark = false;

	UPROPERTY(VisibleAnywhere)
	bool bisFirst = false;

	//스테이지 기록, EStageIndex로 각 스테이지에 접근할 수 있다.
	UPROPERTY(VisibleAnywhere)
	TMap<EStageIndex, FStageRecord> StageRecord;
	//설정 데이터를 저장해둔다.
	UPROPERTY(VisibleAnywhere)
	FSettingData SettingData;
	UPROPERTY()
	//게임 데이터 에셋
	class UGameDataAsset* GameData;
};
