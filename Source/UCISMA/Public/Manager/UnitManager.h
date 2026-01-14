// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "UnitManager.generated.h"

UCLASS()
class UCISMA_API AUnitManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitManager();
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterBeatIndex);
	void Init(class AStageManager* InputStageManager);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class AStageManager* StageManager;
	/*** 비트 유닛 관련 ***/
	//현 스테이지에 존재하는 비트 유닛에 대한 배열
	TArray<AActor*> BeatUnits;
	//다음 스폰해야 하는 비트 유닛의 인덱스
	int BeatUnitSpawnIndex;
	//현재 스테이지에 살아있는 유닛들
	TDoubleLinkedList<AActor*> LivingBeatUnits;

	//조건문 함수
	bool IsUnitPastBeat(class IBeatUnitInterface* BeatUnit, int QuaterBeatIndex);
	bool IsUnitCurrentBeat(class IBeatUnitInterface* BeatUnit, int QuaterBeatIndex);



};
