// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatUnit/Enemy/BeatUnitCharacter_Boss_Harmyth.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ABeatUnitCharacter_Boss_Harmyth::ABeatUnitCharacter_Boss_Harmyth()
{
    SetEnemyType(EEnemyType::Boss);

    BossMesh = GetMesh();

    HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
    HeadMesh->SetupAttachment(BossMesh);
    HeadAuraMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyAuraMesh"));
    HeadAuraMesh->SetupAttachment(HeadMesh);
    HeadAuraMesh->SetRelativeScale3D(FVector(1.05f, 1.1f, 0.01f));
    HeadAuraMesh->SetRelativeLocation(FVector(0, -5.0f, -50.0f));
    BodyCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyCollision"));
    BodyCollision->SetupAttachment(HeadMesh);

    LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
    LeftHandMesh->SetupAttachment(BossMesh);
    LeftHandAuraMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandAuraMesh"));
    LeftHandAuraMesh->SetupAttachment(LeftHandMesh);
    LeftHandAuraMesh->SetRelativeScale3D(FVector(1.05f, 1.05f, 0.01f));
    LeftHandAuraMesh->SetRelativeLocation(FVector(0, -10.0f, -10.0f));
    LeftHandCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandCollision"));
    LeftHandCollision->SetupAttachment(LeftHandMesh);
    LeftHandNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftHandNiagaraComponent"));
    LeftHandNiagaraComponent->SetupAttachment(LeftHandMesh);

    RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
    RightHandMesh->SetupAttachment(BossMesh);
    RightHandAuraMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandAuraMesh"));
    RightHandAuraMesh->SetupAttachment(RightHandMesh);
    RightHandAuraMesh->SetRelativeScale3D(FVector(1.05f, 1.05f, 0.01f));
    RightHandAuraMesh->SetRelativeLocation(FVector(0, -10.0f, -10.0f));
    RightHandCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandCollision"));
    RightHandCollision->SetupAttachment(RightHandMesh);
    RightHandNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightHandNiagaraComponent"));
    RightHandNiagaraComponent->SetupAttachment(RightHandMesh);

    PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
    PostProcessComponent->SetupAttachment(RootComponent); // 보스 캐릭터에 부착

    FrontArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontArrow"));
    FrontArrow->SetupAttachment(GetCapsuleComponent());

    PrimaryActorTick.bCanEverTick = true;
    SetBossState(EBossState::None);

    bIsCamToBossInitialized = false;

    InterpTime = 5.0f;
    EnTime = 0.0f;
}

void ABeatUnitCharacter_Boss_Harmyth::BeginPlay()
{
    Super::BeginPlay();

    InitBossData();
    InitHealthBar();
    InitMaterial();
    DeactivePostProcess();

    BossIndex = FMath::Max(BossIndex, GetBeatIndex());

    PostProcessMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/FX/PostProcess/M_BossGrowggy_Inst.M_BossGrowggy_Inst"));
    if (RightHandNiagaraComponent != nullptr && LeftHandNiagaraComponent != nullptr) {
        LOG_KHJ(Error, TEXT("deactivate"));
        RightHandNiagaraComponent->Deactivate();
        LeftHandNiagaraComponent->Deactivate();
    }

    if (LeftHandCollision && RightHandCollision && BodyCollision)
    {
        LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeatUnitCharacter_Boss_Harmyth::OverlapHand);
        RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeatUnitCharacter_Boss_Harmyth::OverlapHand);
        BodyCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeatUnitCharacter_Boss_Harmyth::OverlapHand);
    }
    else {
        LOG_KHJ(Error, TEXT("보스 오버랩 이벤트 연결 실패"));
    }

    //한비트에 걸리는 시간 가져오기
    AStageManager* stageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
    if (stageManager)
    {
        BeatDuration = stageManager->GetSecondPerBeat() * 1;
    }
    PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    BindToStageManager(stageManager);
}

void ABeatUnitCharacter_Boss_Harmyth::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetIsLiving() || !CurrentStageManager->GetIsStartStage()) return;

    ChasingCamera();
    ProcessWaitQueue(DeltaTime);
    ProcessMoveQueue(DeltaTime);
    UpdateFlashFade(DeltaTime);
}

void ABeatUnitCharacter_Boss_Harmyth::ChasingCamera()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) {
        LOG_KHJ(Error, TEXT("NO Player Controller!"));
    }
    //LOG_KHJ(Error, TEXT("BossMove!"));

    if (!bIsCamToBossInitialized)
    {
        InitCameraDistance(PC);
    }
    else {
        //카메라 위치 파악
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

        // 레이저 위치 업데이트: 카메라 위치에서 일정 거리 유지
        // 카메라->레이저 거리 + 카메라 위치 = 레이저 위치
        FVector NewLaserLocation = CameraLocation + CamToBoss;
        SetActorLocation(NewLaserLocation);
    }
}

//MoveAction에 있는 변수값을 계산해서 MoveQueue에 넣음
void ABeatUnitCharacter_Boss_Harmyth::EnMoveQueue(const FBossMove& MoveAction)
{
    TArray<USkeletalMeshComponent*> TargetMeshes = GetMeshComponentByType(MoveAction.meshType);

    if (TargetMeshes.Num() == 0) return;

    for (USkeletalMeshComponent* TargetMesh : TargetMeshes)
    {
        if (!TargetMesh) continue;

        FBossMove NewMoveAction = MoveAction; // 원본 복사
        float BeatTime = NewMoveAction.beatTime * BeatDuration;

        float StartBeat = NewMoveAction.beatIndex;
        float EndBeat = StartBeat + NewMoveAction.beatTime;
        float StartTime = StartBeat * BeatDuration;
        float EndTime = EndBeat * BeatDuration;

        NewMoveAction.TargetMesh = TargetMesh;
        NewMoveAction.ElapsedTime = 0.0f;
        NewMoveAction.TotalTime = BeatTime;

        if (NewMoveAction.moveType == EBossMoveType::Vector || NewMoveAction.moveType == EBossMoveType::Teleport)
        {
            float ReservedEnd = MoveEndTimeMap_Position.FindRef(TargetMesh);

            if (StartTime < ReservedEnd)
            {
                WaitQueue.Add(NewMoveAction);
                continue;
            }

            // 이동 준비
            FVector CurrentLocation = TargetMesh->GetRelativeLocation();
            NewMoveAction.StartPosition = CurrentLocation;
            NewMoveAction.vectorValue += CurrentLocation;

            MoveEndTimeMap_Position.FindOrAdd(TargetMesh) = EndTime;
        }
        else if (NewMoveAction.moveType == EBossMoveType::Rotate || NewMoveAction.moveType == EBossMoveType::TeleportRota)
        {
            float ReservedEnd = MoveEndTimeMap_Rotation.FindRef(TargetMesh);

            if (StartTime < ReservedEnd)
            {
                WaitQueue.Add(NewMoveAction);
                continue;
            }

            // 회전 준비
            FRotator CurrentRotation = TargetMesh->GetRelativeRotation();
            NewMoveAction.rotationValue += CurrentRotation;

            MoveEndTimeMap_Rotation.FindOrAdd(TargetMesh) = EndTime;
        }

        // 이 FBossMove는 어떤 메시를 위한 것인지 추적 필요
        // 방법 1: FBossMove 안에 TargetMesh 포인터 저장
        LOG_KHJ(Display, TEXT("Boss Move - %s "),
            *StaticEnum<EBossMeshType>()->GetDisplayNameTextByValue((int64)MoveAction.meshType).ToString());

        // 큐에 추가
        MoveQueue.Add(NewMoveAction);

        LOG_KHJ(Display, TEXT("큐 등록: %s 시작: %.2f 종료: %.2f"),
            *TargetMesh->GetName(), StartTime, EndTime);
    }
}


void ABeatUnitCharacter_Boss_Harmyth::ProcessMoveQueue(float DeltaTime)
{
    for (int32 i = 0; i < MoveQueue.Num(); ++i)
    {
        if (!MoveQueue.IsValidIndex(i)) return;

        FBossMove& MoveAction = MoveQueue[i];
        USkeletalMeshComponent* TargetMesh = MoveAction.TargetMesh;
        if (TargetMesh == nullptr) continue;
        
        MoveAction.ElapsedTime += DeltaTime;
        float Alpha = FMath::Clamp(MoveAction.ElapsedTime / MoveAction.TotalTime, 0.0f, 1.0f);

        if (MoveAction.moveType == EBossMoveType::Vector) {
            FVector NewPosition = FMath::Lerp(MoveAction.StartPosition, MoveAction.vectorValue, Alpha);

            TargetMesh->SetRelativeLocation(NewPosition);
            // float Distance = ToPlayerDir.Size(); // 실제 거리가 필요 없고 항상 1.0이므로 주석 처리하거나 제거할 수 있습니다.

            //파티클 발사 조건 추가
            if (MoveAction.bTriggeredEffect)
            {
                SetNiagaraComponent(TargetMesh, MoveAction.beatTime);
                MoveAction.bTriggeredEffect = false;
            }
        }
        else if (MoveAction.moveType == EBossMoveType::Rotate)
        {
            FQuat StartQuat = TargetMesh->GetRelativeRotation().Quaternion();
            FQuat TargetQuat = MoveAction.rotationValue.Quaternion();

            FQuat InterpolatedQuat = FQuat::Slerp(StartQuat, TargetQuat, Alpha);
            TargetMesh->SetRelativeRotation(InterpolatedQuat.Rotator());
        }
        else if (MoveAction.moveType == EBossMoveType::Teleport)
        {
            TargetMesh->SetRelativeLocation(MoveAction.vectorValue);
            LOG_KHJ(Display, TEXT("Vector 위치: %s"), *MoveAction.vectorValue.ToString());
            Alpha = 1;
        }
        else if (MoveAction.moveType == EBossMoveType::TeleportRota)
        {
            FQuat TargetQuat = MoveAction.rotationValue.Quaternion();
            TargetMesh->SetRelativeRotation(TargetQuat.Rotator());
            Alpha = 1;
        }

        if (Alpha >= 1.0f)
        {
            MoveQueue.RemoveAt(i);
            --i;

            TargetMesh->SetMaterial(0, BlackMaterial);

            //만약 둠칫(고개 끄덕이는 거)중 고개가 앞으로 나가있으면
            //다시 집어 넣기
            if (TargetMesh == HeadMesh && bisDance && 
                MoveAction.moveType == EBossMoveType::Vector) StartMoveBackward();
            
            if (TargetMesh == LeftHandMesh)
            {
                LeftHandNiagaraComponent->Deactivate();
            }
            else if (TargetMesh == RightHandMesh) {
                RightHandNiagaraComponent->Deactivate();
            }

        }
    }
}

void ABeatUnitCharacter_Boss_Harmyth::ProcessWaitQueue(float deltaTime)
{
    float CurrentTime = CurrentStageManager->GetBeatIndex() * BeatDuration;

    for (int32 i = 0; i < WaitQueue.Num(); ++i)
    {
        FBossMove& Move = WaitQueue[i];

        USkeletalMeshComponent* TargetMesh = Move.TargetMesh;
        if (!TargetMesh) continue;

        float StartTime = (Move.beatIndex) * BeatDuration;
        float EndTime = StartTime + Move.beatTime * BeatDuration;
         
        if (Move.moveType == EBossMoveType::Vector || Move.moveType == EBossMoveType::Teleport)
        {
            float ReservedEnd = MoveEndTimeMap_Position.FindRef(TargetMesh);

            if (CurrentTime >= StartTime && CurrentTime >= ReservedEnd)
            {
                MoveQueue.Add(Move);
                MoveEndTimeMap_Position.FindOrAdd(TargetMesh) = EndTime;
                WaitQueue.RemoveAt(i);
                --i;
            }
        }
        else if (Move.moveType == EBossMoveType::Rotate || Move.moveType == EBossMoveType::TeleportRota)
        {
            float ReservedEnd = MoveEndTimeMap_Rotation.FindRef(TargetMesh);

            if (CurrentTime >= StartTime && CurrentTime >= ReservedEnd)
            {
                MoveQueue.Add(Move);
                MoveEndTimeMap_Rotation.FindOrAdd(TargetMesh) = EndTime;
                WaitQueue.RemoveAt(i);
                --i;
            }
        }
    }
}

void ABeatUnitCharacter_Boss_Harmyth::ActiveUnit()
{
    Super::ActiveUnit();
    SetBossState(EBossState::Idle);

    if (HeadMesh && HeadMesh->GetMaterial(0))
    {
        //LOG_KHJ_SCREEN(3.f, TEXT("색"));
        //InitMaterial();

        // 투명도 초기값 설정
        CurrentOpacity = 0.0f;

        SetFade(CurrentOpacity);

        // 서서히 등장 시작
        GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &ABeatUnitCharacter_Boss_Harmyth::FadeInMesh, 0.01f, true);
    }

    if (healthBarWidgetClass)
    {
        bossHealthBar = CreateWidget<UWidget_BossHealthBar>(GetWorld(), healthBarWidgetClass);
        if (bossHealthBar)
        {
            bossHealthBar->AddToViewport();
            bossHealthBar->UpdateHealthBar(CurrentHealth, MaxHealth);
        }
    }
    else {
        LOG_KHJ_SCREEN(3.f, TEXT("체력없음"));
    }
}

void ABeatUnitCharacter_Boss_Harmyth::DeactiveUnit()
{

    Super::DeactiveUnit();
    //SetIsLiving(false);

    //SetActorEnableCollision(false);
    ////SetActorTickEnabled(false);
    //SetActorHiddenInGame(true);
    //LOG_KHJ_SCREEN(3.f, TEXT("보스 삭제"));
}

//현재 보스 스테이트
void ABeatUnitCharacter_Boss_Harmyth::SetBossState(EBossState Newstate)
{
    BossState = Newstate;
}

//체력 1을 낮추는 함
void ABeatUnitCharacter_Boss_Harmyth::IsHit(int amount)
{
    if (BossState != EBossState::Idle) return;

    // 추가 로직 작성
    CurrentHealth -= amount;

    if (bossHealthBar) {
        bossHealthBar->UpdateHealthBar(CurrentHealth, MaxHealth);
        bossHealthBar->DamageIndicator();
    }
    else {
        LOG_KHJ_SCREEN(3.f, TEXT("No HealthBar"));
    }

    if (CurrentHealth <= 0)
    {
        Die();
        //여기서 무슨 함수 같은거 뿅
    }
}

//플레이어한테 맞는 함수
void ABeatUnitCharacter_Boss_Harmyth::TakeDamagePlayer(float DamageAmount, EJudgementResult Judgement, AActor* DamageCauser)
{
    LOG_KHJ(Display, TEXT("[Attack Log] Damage: %.1f | Judgement: %d | Causer: %s"),
        DamageAmount,
        static_cast<int32>(Judgement),
        *DamageCauser->GetName()
    );
    IsHit(CurrentHealth);

}

void ABeatUnitCharacter_Boss_Harmyth::InitBossData()
{
    beatDataAsset = LoadObject<UBoss_Harmyth_DataAsset>(nullptr, TEXT("/Game/Data/BossHarmythAsset.BossHarmythAsset"));
    if (beatDataAsset == nullptr) {
        LOG_KHJ(Error, TEXT("No bossAssetData"));
        return;
    }
    this->MaxHealth     = beatDataAsset->maxHealth;
    this->CurrentHealth = beatDataAsset->maxHealth;
    this->damage        = beatDataAsset->damage;
}

void ABeatUnitCharacter_Boss_Harmyth::BeatUpdate(int QuaterIndex)
{

    if (QuaterIndex == 0) {
        BossIndex++;

        //둠칫둠칫은 에러가 있어서 잠시 주석
        if (BossIndex % beatDataAsset->beatBossDance.HeadDanceCycle == 0 && !bisDance)
        {
            StartMoveForward();
        }

        for (const FBossMove& Action : beatDataAsset->beatBossMove)
        {
            //보스 인덱스 + 시작 인덱스 = 현재 인덱스
            if (Action.beatIndex == BossIndex)
            {
                EnMoveQueue(Action);
            }
        }
        if (beatDataAsset->groggyBeatIndex == BossIndex) ActivePostProcess();
        if (beatDataAsset->CheckFailedBeat == BossIndex) OnBossKillFailed();
    }
}

void ABeatUnitCharacter_Boss_Harmyth::StartMoveForward()
{
    if (!HeadMesh || !beatDataAsset) return;

    FBossDance beatBossDance = beatDataAsset->beatBossDance;

    FVector ForwardDir = GetActorForwardVector();
    FVector Offset = ForwardDir * beatBossDance.HeadMoveDistance;
    bisDance = true;

    FBossMove HeadMove;
    HeadMove.beatIndex = BossIndex;
    HeadMove.meshType = EBossMeshType::Head;
    HeadMove.moveType = EBossMoveType::Vector;
    HeadMove.vectorValue = ForwardDir * beatBossDance.HeadMoveDistance;
    HeadMove.beatTime = beatBossDance.HeadMoveTime;

    EnMoveQueue(HeadMove);
}

void ABeatUnitCharacter_Boss_Harmyth::StartMoveBackward()
{
    FBossDance beatBossDance = beatDataAsset->beatBossDance;

    FVector ForwardDir = GetActorForwardVector();
    FVector Offset = ForwardDir * beatBossDance.HeadMoveDistance;
    bisDance = false;

    FBossMove HeadMove;
    HeadMove.beatIndex = BossIndex;
    HeadMove.meshType = EBossMeshType::Head;
    HeadMove.moveType = EBossMoveType::Vector;
    HeadMove.vectorValue = ForwardDir * beatBossDance.HeadMoveDistance * -1;
    HeadMove.beatTime = beatBossDance.HeadReMoveTime;

    EnMoveQueue(HeadMove);
}

void ABeatUnitCharacter_Boss_Harmyth::OverlapHand(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //LOG_KHJ_SCREEN(3.f, TEXT("오버랩"));

    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (!Player) return;


    if (OtherComp && OtherComp->ComponentHasTag(TEXT("Sword"))) return;
    else if (OtherComp && OtherComp->ComponentHasTag(TEXT("HitRange"))) {
        LOG_KHJ(Display, TEXT("[HitCheck] OverlapComponent: %s, HitActor: %s | HitComponent: %s"),
            *OverlappedComponent->GetName(),
            *OtherActor->GetName(),
            *OtherComp->GetName());

        if (CanDealDamageFromPart(OverlappedComponent))
        {
            Player->Take_HP(-1.0f, this);
            LOG_KHJ(Display, TEXT("Damage from %s allowed"), *GetNameSafe(OverlappedComponent));
        }
        else
        {
            LOG_KHJ(Display, TEXT("🟦 Damage from %s denied"), *GetNameSafe(OverlappedComponent));
        }
    }
}

//지금 떄릴 수있는 시간인지 판별하는 함수
bool ABeatUnitCharacter_Boss_Harmyth::bIsHitTime()
    {
        auto& startTime = beatDataAsset->beatBossHit.HitStartBeat;
        auto& endTime = beatDataAsset->beatBossHit.HitEndBeat;

        return startTime <= BossIndex && BossIndex <= endTime;
    }

bool ABeatUnitCharacter_Boss_Harmyth::SetNiagaraComponent(USkeletalMeshComponent* targetMesh, float time)
{
    if (targetMesh == nullptr) return false;

    if (targetMesh == LeftHandMesh)
    {
        LeftHandNiagaraComponent->SetVariableFloat(FName("lifetime"), time);
        LeftHandNiagaraComponent->Activate();
        return true;
    }
    else if (targetMesh == RightHandMesh) {
        RightHandNiagaraComponent->SetVariableFloat(FName("lifetime"), time);
        RightHandNiagaraComponent->Activate();
        return true;
    }

    //LOG_KHJ(Error, TEXT("알 수 없는 메시임: %s"), *targetMesh->GetFullName());
    return false;
}

void ABeatUnitCharacter_Boss_Harmyth::Die()
{
    CurrentStageManager->BossDeadHandler();
    PlayScreenFadeEffect();
    SetBossState(EBossState::Disappear);

    bossHealthBar->DestroyHealthBar();
}

void ABeatUnitCharacter_Boss_Harmyth::PlayScreenFadeEffect()
{
    if (ScreenFadeWidgetClass)
    {
        ScreenFadeWidget = CreateWidget<UWidget_ScreenFade>(GetWorld(), ScreenFadeWidgetClass);
        if (ScreenFadeWidget)
        {
            ScreenFadeWidget->AddToViewport(1);
            ScreenFadeWidget->PlayFade();
        }
    }
}

void ABeatUnitCharacter_Boss_Harmyth::OnBossKillFailed()
{
    if (IsBossStillAlive())
    {
        CurrentStageManager->GameOver(EGameOverType::GameFailed_PlayerDied);
    }
}

bool ABeatUnitCharacter_Boss_Harmyth::IsBossStillAlive()
{
    //0보다 작으면 true
    return CurrentHealth <= 0 ? false : true;
}


void ABeatUnitCharacter_Boss_Harmyth::InitHealthBar()
{
    ScreenFadeWidgetClass = LoadClass<UWidget_ScreenFade>(nullptr, TEXT("/Game/Assets/BGIllustration/w/WBP_FadeScreen.WBP_FadeScreen_C"));
    healthBarWidgetClass = LoadClass<UWidget_BossHealthBar>(nullptr, TEXT("/Game/UI/wbp_BossHealthBar.wbp_BossHealthBar_C"));
}

//처음 시작하고 카메라랑 보스 사이 거리 측정
void ABeatUnitCharacter_Boss_Harmyth::InitCameraDistance(APlayerController* PC)
{
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
    CamToBoss = GetActorLocation() - CameraLocation;


    SetBossState(EBossState::Idle);
    bIsCamToBossInitialized = true;
}

void ABeatUnitCharacter_Boss_Harmyth::BindToStageManager(AStageManager* StageMgr)
{
    if (StageMgr)
    {
        // 🔹 기존 StageManager 연결 해제
        if (CurrentStageManager)
        {
            CurrentStageManager->OnPurificationSuccess.RemoveAll(this);;
            LOG_KHJ(Error, TEXT("Unbound from previous StageManager event."));
        }

        // 🔹 새로운 StageManager에 바인딩
        CurrentStageManager = StageMgr;
        CurrentStageManager->OnPurificationSuccess.AddUFunction(this, FName("PurificationDamage"));
        LOG_KHJ(Error, TEXT("Successfully bound to new StageManager event."));
    }
}

void ABeatUnitCharacter_Boss_Harmyth::PurificationDamage()
{
    OnHitFlashEffect();
    IsHit(1);
}

//해당되는 메시가 뭔지 알려주는 함수, Enum = EBossMeshType
TArray<USkeletalMeshComponent*> ABeatUnitCharacter_Boss_Harmyth::GetMeshComponentByType(EBossMeshType MeshType)
{
    TArray<USkeletalMeshComponent*> Result;

    switch (MeshType)
    {
    case EBossMeshType::LeftHand:
        Result.Add(LeftHandMesh);
        break;

    case EBossMeshType::RightHand:
        Result.Add(RightHandMesh);
        break;

    case EBossMeshType::BothHands:
        Result.Add(LeftHandMesh);
        Result.Add(RightHandMesh);
        break;

    case EBossMeshType::Head:
        Result.Add(HeadMesh);
        break;

    case EBossMeshType::Body:
        Result.Add(BossMesh);
        break;

    default:
        LOG_KHJ_SCREEN(3.f, TEXT("아무것도 선택되지 않음"));
        break;
    }

    return Result;
}

void ABeatUnitCharacter_Boss_Harmyth::InitMaterial()
{
    constexpr int32 SLOT_BLACK = 0;
    constexpr int32 SLOT_EYE = 1;
    //동적 머터리얼 가져오기
    BlackMaterial = UMaterialInstanceDynamic::Create(HeadMesh->GetMaterial(0), this);
    EyeMaterial = UMaterialInstanceDynamic::Create(HeadMesh->GetMaterial(1), this);
    AuraMaterial = UMaterialInstanceDynamic::Create(HeadAuraMesh->GetOverlayMaterial(), this);
    //동적 머터리얼로 다시 설정하기
    HeadMesh->SetMaterial(SLOT_BLACK, BlackMaterial);
    HeadMesh->SetMaterial(SLOT_EYE, EyeMaterial);
    LeftHandMesh->SetMaterial(SLOT_BLACK, BlackMaterial);
    RightHandMesh->SetMaterial(SLOT_BLACK, BlackMaterial);
    //동적 오버레이 머터리얼 설정
    HeadAuraMesh->SetOverlayMaterial(AuraMaterial);
    LeftHandAuraMesh->SetOverlayMaterial(AuraMaterial);
    RightHandAuraMesh->SetOverlayMaterial(AuraMaterial);
}

void ABeatUnitCharacter_Boss_Harmyth::SetFade(int Value)
{
    BlackMaterial->SetScalarParameterValue(TEXT("Fade"), Value);
    EyeMaterial->SetScalarParameterValue(TEXT("Fade"), Value);
    AuraMaterial->SetScalarParameterValue(TEXT("Fade"), Value);
}

void ABeatUnitCharacter_Boss_Harmyth::FadeInMesh()
{
    // 투명도를 점진적으로 증가
    CurrentOpacity += 0.0033f;

    // 머티리얼 파라미터 업데이트
    BlackMaterial->SetScalarParameterValue(TEXT("Fade"), CurrentOpacity);
    EyeMaterial->SetScalarParameterValue(TEXT("Fade"), CurrentOpacity);
    AuraMaterial->SetScalarParameterValue(TEXT("Fade"), CurrentOpacity);

    // Opacity 값 제한
    if (CurrentOpacity >= 1.0f)
    {
        CurrentOpacity = 1.0f;
        //DynamicMaterial = BossMaterial;
        // 타이머 중지 
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
    }
}

void ABeatUnitCharacter_Boss_Harmyth::ActivePostProcess()
{
    if (PostProcessComponent && !beingRunPost && !beingDecayPost)
    {

        // 이미 생성된 경우 중복 생성 방지
        if (!DynamicMaterial)
        {
            if (!PostProcessMaterial)
            {
                return;
            }
            
            DynamicMaterial = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
            if (!DynamicMaterial)
            {
                return;
            }
        }

        // 기존 후처리 머티리얼 제거 후 적용
        PostProcessComponent->Settings.WeightedBlendables.Array.Empty();
        PostProcessComponent->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, DynamicMaterial));

        // ColorSize(Radius) 파라미터 초기화
        DynamicMaterial->SetScalarParameterValue(FName("Radius"), 0.0f);

        PostProcessComponent->bEnabled = true;
        EnTime = 0;
        InterpTime = beatDataAsset->groggyRunningBeat * BeatDuration;

        GetWorld()->GetTimerManager().SetTimer(PostProcessTimerHandle, this, &ABeatUnitCharacter_Boss_Harmyth::UpdatePostProcessEffect, 0.01f, true);
        beingRunPost = true;
        beingDecayPost = false;  // 초기화

        // 0.1초마다 값 업데이트 (프레임 독립적 보간)
        //GetWorld()->GetTimerManager().SetTimer(PostProcessTimerHandle, this, &ABeatUnitCharacter_Boss_Harmyth::UpdatePostProcessEffect, 0.1f, true);
    }
}

void ABeatUnitCharacter_Boss_Harmyth::UpdatePostProcessEffect()
{

    if (InterpTime <= 0.0f || !DynamicMaterial || !PostProcessComponent || !IsValid(PostProcessComponent))
    {
        GetWorld()->GetTimerManager().ClearTimer(PostProcessTimerHandle);
        return;
    }

    EnTime += 0.01;

    if (EnTime >= 100.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(PostProcessTimerHandle);
        return;
    }

    float NewRadius = 0.0f;

    if (beingRunPost)
    {
        NewRadius = FMath::Lerp(CurrentRadius, TargetRadius, EnTime / InterpTime);
        if (EnTime >= InterpTime)
        {
            beingRunPost = false;
            beingDecayPost = true;
            EnTime = 0;
            InterpTime = beatDataAsset->groggyDecayTime * BeatDuration;
        }
    }
    else if (beingDecayPost)
    {
        NewRadius = FMath::Lerp(TargetRadius, CurrentRadius, EnTime / InterpTime);
        if (EnTime >= InterpTime)
        {
            GetWorld()->GetTimerManager().ClearTimer(PostProcessTimerHandle);
            DeactivePostProcess();
            beingDecayPost = false;
        }
    }

    if (FMath::IsNaN(NewRadius))
    {
        GetWorld()->GetTimerManager().ClearTimer(PostProcessTimerHandle);
        return;
    }

    DynamicMaterial->SetScalarParameterValue(FName("Radius"), NewRadius);
}

void ABeatUnitCharacter_Boss_Harmyth::DeactivePostProcess()
{
    if (PostProcessComponent)
    {
        PostProcessComponent->bEnabled = false;
    }
}

void ABeatUnitCharacter_Boss_Harmyth::OnHitFlashEffect()
{
    float Flash = FlashIntensity;

    LOG_KHJ(Error, TEXT("Flash : %d"), Flash);

    BlackMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);
    EyeMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);
    AuraMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);

    FlashElapsedTime = 0.f;
    bIsFlashing = true;
}

void ABeatUnitCharacter_Boss_Harmyth::UpdateFlashFade(float delta)
{
    if (!bIsFlashing) return;

    FlashElapsedTime += delta;
    float Alpha = 1.f - (FlashElapsedTime / BeatDuration);
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    float Flash = FlashIntensity * Alpha;

    if (BlackMaterial) BlackMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);
    if (EyeMaterial)   EyeMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);
    if (AuraMaterial)  AuraMaterial->SetScalarParameterValue(FName("FlashAmount"), Flash);

    if (Alpha <= 0.f)
    {
        BlackMaterial->SetScalarParameterValue(FName("FlashAmount"), 0);
        EyeMaterial->SetScalarParameterValue(FName("FlashAmount"), 0);
        AuraMaterial->SetScalarParameterValue(FName("FlashAmount"), 0);
        bIsFlashing = false;
    }
}

void ABeatUnitCharacter_Boss_Harmyth::OnDeath()
{
    LOG_KHJ_SCREEN(3.0f, TEXT("Boss Down!!"));
    Destroy();
}

void ABeatUnitCharacter_Boss_Harmyth::SetBossIndex(int Index)
{
    BossIndex = Index;
    BossIndex = FMath::Max(0, Index);
}

void ABeatUnitCharacter_Boss_Harmyth::BossSetupStageSkip(int StartBeatIndex)
{
    SetBossIndex(StartBeatIndex);
    // TODO: 이중으로 BeatDuration을 정하기 때문에 최적화 방법을 고민해야 한다.
    AStageManager* stageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
    if (stageManager)
    {
        BeatDuration = stageManager->GetSecondPerBeat() * 1;
    }
    // 보스의 위치를 (보스 시작 위치) -> (시작 타일 위치)를 하여 보스가 화면 내에 위치하도록 만든다.
    ACamera_Map* Camera = stageManager->GetCamera();
    int BossStartTileIndex = Camera->BeatIndexToTileIndex(GetBeatIndex());
    FVector BossStartTileLocation = Camera->Tile_Position[BossStartTileIndex];
    FVector PlayerStartTileLocation = Camera->Tile_Position[stageManager->GetStageStartIndex()];
    FVector BossStartLocation = GetActorLocation() + (PlayerStartTileLocation - BossStartTileLocation);
    SetActorLocation(BossStartLocation);
    //스킵되는 비트 인덱스까지의 행동을 실행한다.
    for (const FBossMove& Action : beatDataAsset->beatBossMove)
    {
        //보스 인덱스 + 시작 인덱스 = 현재 인덱스
        if (Action.beatIndex <= StartBeatIndex)
        {
            FBossMove NowAction = Action;
            NowAction.beatTime = 0.001f;
            EnMoveQueue(NowAction);
            ProcessMoveQueue(0.1f);
        }
    }
    
    

    bIsCamToBossInitialized = false;
}

bool ABeatUnitCharacter_Boss_Harmyth::CanDealDamageFromPart(UPrimitiveComponent* OverlappedComponent)
{
    const EBossBodyPart HitPart = GetPartTypeFromPriComponent(OverlappedComponent);

    LOG_KHJ(Display, TEXT("🔍 %d [DamageCheck] HitComponent: %s | HitPart: %s"),
        BossIndex,
        *OverlappedComponent->GetName(),
        *StaticEnum<EBossBodyPart>()->GetDisplayNameTextByValue((int64)HitPart).ToString());

    for (const FBossMove& Move : MoveQueue)
    {
        if (!Move.TargetMesh)
        {
            LOG_KHJ(Display, TEXT("⛔ Move.TargetMesh is nullptr — Skipping"));
            continue;
        }

        // 대상 메시 일치 여부 (선택적: 메시 매칭 조건 넣을 수도 있음)
        const bool bIsTargetMatch = (OverlappedComponent == Move.TargetMesh);
        LOG_KHJ(Display, TEXT("→ Checking Move for Mesh: %s (Match: %s)"),
            *Move.TargetMesh->GetName(),
            bIsTargetMatch ? TEXT("true") : TEXT("false"));

        const TArray<FBossMeshDamageInfo>& UsedDamageInfos =
            (Move.DamageInfos.Num() > 0) ? Move.DamageInfos : beatDataAsset->DefaultDamageInfos;

        for (const FBossMeshDamageInfo& Info : UsedDamageInfos)
        {
            FString InfoPart = StaticEnum<EBossBodyPart>()->GetDisplayNameTextByValue((int64)Info.PartType).ToString();
            FString CanDeal = Info.bCanDealDamage ? TEXT("✅") : TEXT("❌");

            LOG_KHJ(Display, TEXT("   └ DamageInfo: %s = %s"), *InfoPart, *CanDeal);

            if (Info.PartType == HitPart && Info.bCanDealDamage)
            {
                LOG_KHJ(Display, TEXT("🎯 [Damage Granted] Part: %s (From Move: %s)"),
                    *InfoPart, *Move.TargetMesh->GetName());
                return true;
            }
        }
    }

    LOG_KHJ(Display, TEXT("🚫 [Damage Denied] No matching damage info for: %s"),
        *StaticEnum<EBossBodyPart>()->GetDisplayNameTextByValue((int64)HitPart).ToString());

    return false;
}
EBossBodyPart ABeatUnitCharacter_Boss_Harmyth::GetPartTypeFromPriComponent(UPrimitiveComponent* Comp)
{
    if (Comp == LeftHandCollision)     return EBossBodyPart::LeftHand;
    if (Comp == RightHandCollision)    return EBossBodyPart::RightHand;
    if (Comp == BodyCollision)         return EBossBodyPart::Head;
    return EBossBodyPart::None;
}

