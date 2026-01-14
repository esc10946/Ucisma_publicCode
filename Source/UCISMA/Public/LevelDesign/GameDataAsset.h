// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "UCISMA/UCISMA.h"
#include "Engine/DataAsset.h"
#include "Manager/StageManager.h"
#include "System/CommonTypes.h"
#include "GameDataAsset.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FStageData
{
	GENERATED_USTRUCT_BODY()
public:
	FStageData() : Song(nullptr), SongName(TEXT("")), Bpm(0), SongLength(0), MaxPurificationFailureCount(0),CameraSpeed(1),bIsPurifyActive(true){
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	USoundWave* Song;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	FString SongName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	int Bpm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	int SongLength;
	//해당 스테이지에서 실패 기준이 되는 정화 실패 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Record")
	int MaxPurificationFailureCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	int CameraSpeed; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool bIsPurifyActive;

};


UCLASS(BlueprintType)
class UCISMA_API UGameDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	const FStageData GetStageData(EStageIndex StageIndex);
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
	float GetPlayer_Attack_Range() { return Player_Attack_Range; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetPlayer_Dash_Range() { return Player_Dash_Range; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	FSettingData GetDefaultSettingData() { return DefaultSettingData; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	TMap<ESfx, USoundWave*> GetSfx() { return Sfx; };


	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	FString	GetSongName(EStageIndex StageIndex) { return StageData[StageIndex].SongName; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	TArray<FText>& GetTips() { return Tips; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetDividerThicknessMax() const { return DividerThicknessMax; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetDividerThicknessMin() const { return DividerThicknessMin; }

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	float GetDividerInterval() const { return DividerInterval; }


private:
	UPROPERTY(EditAnywhere, Category = "StageData")
	TMap<EStageIndex, FStageData> StageData;


	/*** 판정 ***/
	//퍼펙트 판정 범위, 1비트당 시간에 해당 수치를 곱해서 판정 시간이 정해진다.
	UPROPERTY(EditAnywhere, Category = "GameData|Judgement")
	float PerfectPercentageOfAttack;
	//그레이트 판정 범위, 1비트당 시간에 해당 수치를 곱해서 판정 시간이 정해진다.
	UPROPERTY(EditAnywhere, Category = "GameData|Judgement")
	float GreatPercentageOfAttack;
	//굳 판정 범위, 1비트당 시간에 해당 수치를 곱해서 판정 시간이 정해진다.
	UPROPERTY(EditAnywhere, Category = "GameData|Judgement")
	float GoodPercentageOfAttack;
	//퍼펙트 대쉬 성공 판정 범위, 1비트당 시간에 해당 수치를 곱해서 판정 시간이 정해진다.
	UPROPERTY(EditAnywhere, Category = "GameData|Judgement")
	float PerfectPercentageOfDash;
	//대쉬 성공 판정 범위, 1비트당 시간에 해당 수치를 곱해서 판정 시간이 정해진다.
	UPROPERTY(EditAnywhere, Category = "GameData|Judgement")
	float SuccessPercentageOfDash;

	//게임에서 사용되는 효과음 배열
	UPROPERTY(EditAnywhere, Category = "GameData|Sfx")
	TMap<ESfx, USoundWave*> Sfx;

	//게임의 디폴트 세팅값
	UPROPERTY(EditAnywhere, Category = "GameData|Setting")
	FSettingData DefaultSettingData;

	//플레이어 공격 타겟팅 범위
	UPROPERTY(EditAnywhere, Category = "PlayerData|PlayerState")
	float Player_Attack_Range;
	UPROPERTY(EditAnywhere, Category = "PlayerData|PlayerState")
	float Player_Dash_Range;

	//게임 시작 전 화면에 뜨는 팁 목록
	UPROPERTY(EditAnywhere, Category = "GameData|Tip")
	TArray<FText> Tips;

	//적 체력바 관련 수치
	//체력 기준선 최대두께
	UPROPERTY(EditAnywhere, Category = "GameData|EnemyHealthBar")
	float DividerThicknessMax;
	//체력 기준선 최소두께
	UPROPERTY(EditAnywhere, Category = "GameData|EnemyHealthBar")
	float DividerThicknessMin;
	//체력을 나누는 기준
	UPROPERTY(EditAnywhere, Category = "GameData|EnemyHealthBar")
	float DividerInterval;


};
