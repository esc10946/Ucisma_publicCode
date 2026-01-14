// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "UCISMA/UCISMA.h"
#include "BeatUnitActor.h"
#include "BeatUnitActor_Laser.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API ABeatUnitActor_Laser : public ABeatUnitActor
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void BeatUpdate(int QuaterIndex) override;

    //레이저의 상태 열거형
    enum class ELaserState { None, Preparing, Fired, Disappearing };
    ELaserState LaserState;

    //컴포넌트
    //레이저의 영역(레벨디자인시 보이는 네모박스)
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* LaserMesh;

    UPROPERTY(EditAnywhere)
    UWidgetComponent* LaserImage;

    //레이저 표시선 밝은거
    UPROPERTY(meta = (BindWidget))
    UImage* LaserLine;
    
    UPROPERTY(EditAnywhere, Category = "Design Settings|Color")
    FLinearColor LaserColorBeat1 = FLinearColor::Red;

    UPROPERTY(EditAnywhere, Category = "Design Settings|Color")
    FLinearColor LaserColorBeat2 = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, Category = "Design Settings|Color")
    FLinearColor LaserColorBeat3 = FLinearColor::White;

    UMaterialInstanceDynamic* LaserMaterialInstance;

    //레이저 프로세스바
    UPROPERTY(meta = (BindWidget))
    UProgressBar* LaserBar;

    // 타이밍 관리
    float StateTimer;
    float BeatDuration;
    float TimeLine = 0;

    bool DoOnce;

    // 피해 처리
    UPROPERTY(EditAnywhere)
    float Damage;

    int BeatIdx;

    //카메라 위치
    FVector CameraLocation;
    FRotator CameraRotation;
    FVector CamToLaser;

    const int QUARTERS_PER_BEAT = 4;

    // 함수 /////////////////////////////////////////////////////////////////////
    void InitLaser();
    void CheckStateTime();
    void CheckHit();
    void ChaseCamera(APlayerController* PC);
    void DisappearLaser(float delta);
    void SetLaserState(ELaserState NewState);

    void TakeDamageLaser(AActor* HitActor);

    ABeatUnitActor_Laser();
};