// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Pawn.h"
#include "BeatUnitInterface.h"
#include "BeatUnitPawn.generated.h"


UCLASS()
class UCISMA_API ABeatUnitPawn : public APawn, public IBeatUnitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABeatUnitPawn();
	//비트매니저로부터 1/4비트마다 호출되는 함수
	UFUNCTION()
	virtual void BeatUpdate(int QuaterIndex) override;
	//해당 유닛을 비활성화 하는 함수
	virtual void DeactiveUnit() override;
	//해당 유닛을 활성화 하는 함수
	virtual void ActiveUnit() override;

	/*** Get 함수들 ***/
	virtual const int GetBeatIndex() const override { return BeatIndex; }
	virtual const int GetQuaterBeatIndex() const override { return QuaterBeatIndex; }
	virtual bool GetIsLiving() override { return bIsLiving; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
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