// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Character.h"
#include "BeatUnitInterface.h"
#include "BeatUnitCharacter.generated.h"

// TODO: StageManager가 없는 맵에서 튕기는 거 예외처리

UCLASS()
class UCISMA_API ABeatUnitCharacter : public ACharacter, public IBeatUnitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABeatUnitCharacter();
	//비트매니저로부터 1/4비트마다 호출되는 함수
	UFUNCTION()
	virtual void BeatUpdate(int QuaterIndex) override;
	//해당 유닛을 비활성화 하는 함수
	virtual void DeactiveUnit() override;
	//해당 유닛을 활성화 하는 함수
	virtual void ActiveUnit() override;

	//플레이어에게 정화된 유닛 비활성화 함수
	virtual void LivingUnit();

	/*** Get 함수들 ***/
	virtual const int GetBeatIndex() const override { return BeatIndex; }
	virtual const int GetQuaterBeatIndex() const override { return QuaterBeatIndex; }
	virtual bool GetIsLiving() override { return bIsLiving; }

	void SetBeatIndex(int32 value) { BeatIndex = value; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//소환된 비트에 BeatUpdate를 호출하는지에 대한 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCallBeatUpdateOnSpawn;

private:
	class AStageManager* StageManager;
	//해당 유닛이 맵에 등장하는 비트
	UPROPERTY(EditAnywhere, Category = "Design Settings")
	int BeatIndex;
	//해당 유닛이 맵에 등장하는 비트의 1/4박자 단위
	//만약 유닛이 13.5비트에 소환되면 BeatIndex : 13, QuaterBeatIndex = 2가 된다.
	UPROPERTY(EditAnywhere, Category = "Design Settings")
	int QuaterBeatIndex;
	//해당 유닛의 활성화 여부
	bool bIsLiving;
	


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

 };