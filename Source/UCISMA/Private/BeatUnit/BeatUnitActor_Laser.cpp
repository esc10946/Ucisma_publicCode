// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor_Laser.h"
#include "PlayerCharacter.h"
#include "CollisionQueryParams.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Manager/BeatManager.h"
#include "Manager/StageManager.h"
#include <Kismet/GameplayStatics.h>

ABeatUnitActor_Laser::ABeatUnitActor_Laser()
{

    LaserImage = CreateDefaultSubobject<UWidgetComponent>(TEXT("LaserImage"));
    RootComponent = LaserImage;

    LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
    LaserMesh->SetupAttachment(LaserImage);

    // 기본값 초기화
    LaserState = ELaserState::None;
    StateTimer = 0.0f;
    BeatDuration = 1.0f; // 1비트 기준 시간
    Damage = 1.0f; // 기본 대미지
    DoOnce = true;

    LaserImage->SetCastShadow(false);
    //레이저는 소환과 동시에 비트업데이트 활성화
    bCallBeatUpdateOnSpawn = true;
}

void ABeatUnitActor_Laser::BeginPlay()
{
    Super::BeginPlay();
    InitLaser();
}

void ABeatUnitActor_Laser::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && LaserState != ELaserState::None) ChaseCamera(PC);
    //점차 fadeout되는 레이저 이미지 때문에 여기에
    if (LaserState == ELaserState::Disappearing) DisappearLaser(DeltaTime);
}

void ABeatUnitActor_Laser::BeatUpdate(int QuaterIndex)
{
    StateTimer++;
    //레이저 준비단계
    //레이저와 카메라사이의 거리 계산
    if (LaserState == ELaserState::None) {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
            CamToLaser = GetActorLocation() - CameraLocation;

            SetLaserState(ELaserState::Preparing);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("CamToLaser DIstance : False"));
        }
    }

    CheckStateTime();
    
    if (QuaterIndex) return;
    FLinearColor NewColor = FLinearColor::Red;

    switch (LaserState)
    {
        // 레이저 준비 단계
    case ELaserState::Preparing:
        //이것도 변경하자 -> color를 배열로 비트의 개수를 정해놓고
        //나중에 바꿀수 있게 ㅇㅇ뭔말알?
        if (StateTimer == 0 * QUARTERS_PER_BEAT)        NewColor = LaserColorBeat1;
        else if (StateTimer == 1 * QUARTERS_PER_BEAT)   NewColor = LaserColorBeat2;
        else if (StateTimer >= 2 * QUARTERS_PER_BEAT)   NewColor = LaserColorBeat3;

        LaserLine->SetVisibility(ESlateVisibility::Visible);
        LaserLine->SetColorAndOpacity(NewColor);
        break;
        // 레이저 발사 단계
    case ELaserState::Fired:
        if (StateTimer >= 0) {
            LaserLine->SetVisibility(ESlateVisibility::Hidden);
            LaserBar->SetVisibility(ESlateVisibility::Visible);

            SetLaserState(ELaserState::Disappearing);
          
            CheckHit();
        }
        break;
        // 레이저 소멸 단계
    case ELaserState::Disappearing:
        if (StateTimer > 0.5 * QUARTERS_PER_BEAT) DeactiveUnit();
        break;
    }
}

void ABeatUnitActor_Laser::CheckStateTime()
{
    //레이저 표시선 이미지 변경
    if (LaserState == ELaserState::Preparing || LaserState == ELaserState::Fired)
    {
        if (StateTimer >= 3 * QUARTERS_PER_BEAT)
        {
            SetLaserState(ELaserState::Fired);
        }
        //레이저 프로그레스 바 변경
    }
}

void ABeatUnitActor_Laser::InitLaser()
{
    LaserLine = Cast<UImage>(LaserImage->GetWidget()->GetWidgetFromName(TEXT("LaserLine")));
    LaserBar = Cast<UProgressBar>(LaserImage->GetWidget()->GetWidgetFromName(TEXT("Laser_Bar")));

    UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Enemy/Obstacles/Laser/Asset/M_Laser_UI_Inst.M_Laser_UI_Inst'"));
    LaserMaterialInstance = UMaterialInstanceDynamic::Create(BaseMat, this);

    // 시각화 메시는 삭제
    LaserMesh->SetVisibility(false);
    LaserBar->SetVisibility(ESlateVisibility::Hidden); //widgetcomponent는 SetVisibility 매개변수가 다름

    //한비트에 걸리는 시간 가져오기
    AStageManager* stageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
    if (stageManager)
    {
        BeatDuration = stageManager->GetSecondPerBeat() * 1;
        //UE_LOG(LogTemp, Log, TEXT("BeatDuration set: %f"), BeatDuration);
    }
}

void ABeatUnitActor_Laser::CheckHit()
{
    FVector BoxExtent = LaserMesh->GetRelativeScale3D() * 50; // 메시의 로컬 박스 크기
    FVector Scale = GetActorScale(); // 월드 스케일
    FVector ScaledBoxExtent = BoxExtent * Scale; // 스케일 적용한 박스 크기

    // 충돌 감지 및 피해 처리
    FVector start = GetActorLocation();
    FVector end = start + FVector(0, 0, -200); // 아래 방향으로 이동

    //에디터에서 회전한 정도
    FQuat Quat = GetActorQuat();

    TArray<FHitResult> HitResults;
    FCollisionShape BoxShape = FCollisionShape::MakeBox(ScaledBoxExtent);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,           // 여러 충돌 정보를 저장할 배열
        start,                // 시작 위치
        end,                  // 끝 위치
        Quat,      // 회전 (기본값)
        ECC_GameTraceChannel1,       // 충돌 채널
        BoxShape,             // 박스 모양
        Params                // 충돌 쿼리 매개변수
    );

    //맞았으면 대미지
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            TakeDamageLaser(HitActor);
        }
    }
}

void ABeatUnitActor_Laser::ChaseCamera(APlayerController* PC)
{
    //카메라 위치 파악
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // 레이저 위치 업데이트: 카메라 위치에서 일정 거리 유지
    FVector NewLaserLocation = CameraLocation + CamToLaser; // 카메라->레이저 거리 + 카메라 위치 = 레이저 위치

    SetActorLocation(NewLaserLocation);
    FVector EndPoint = CameraLocation + CameraRotation.Vector() * 500.0f; // 카메라 방향 500 단위 거리까지 확인하기
}

void ABeatUnitActor_Laser::DisappearLaser(float delta)
{
    //Destroy(); // 레이저 제거
    if (StateTimer <= 0.5 * QUARTERS_PER_BEAT)
    {
        TimeLine += delta;
        float alpha = FMath::Clamp(1.0f - (TimeLine / (BeatDuration * 0.5)), 0.0f, 1.0f);
        //UE_LOG(LogTemp, Display, TEXT("alpha = %f"), alpha);
        LaserBar->SetPercent(alpha);
    }
    else
    {
        LaserBar->SetPercent(0.0f); // 값이 0으로 고정
    }
}

void ABeatUnitActor_Laser::SetLaserState(ELaserState NewState)
{
    LaserState = NewState;
    StateTimer = 0.0f;
}

void ABeatUnitActor_Laser::TakeDamageLaser(AActor* HitActor)
{
    //UE_LOG(LogTemp, Warning, TEXT("No Hit : %s"), *HitActor->GetFName().ToString());
    APlayerCharacter* Player = Cast<APlayerCharacter>(HitActor);
    //플레이어가 맞으면 플레이어메소드인 Take_HP호출
    if (Player && DoOnce)
    {
        Player->Take_HP(-Damage, this);

        DoOnce = false;
    }
}