// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "System/CommonTypes.h"
#include "SoundManager.generated.h"

UCLASS()
class UCISMA_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();
	UFUNCTION()
	void Init(class AStageManager* InputStageManager);
	//테스트용
	void StartSong();
	//사운드를 일시정지하는 함수
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PauseSong();
	//사운드를 재개하는 함수
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ResumeSong();
	void PlaySfx(ESfx SfxType);
	void SetSongStartTime(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class AStageManager* StageManager;

	/*** 노래 관련 ***/
	//노래의 길이, 초 단위
	int SongLength;
	//스테이지 노래
	USoundWave* Song;
	//노래 시작 시간
	float SongStartTime;
	//오디오 컴포넌트
	UPROPERTY()
	UAudioComponent* AudioComponent;
	
	//효과음
	TMap<ESfx, USoundWave*> Sfx;
};
