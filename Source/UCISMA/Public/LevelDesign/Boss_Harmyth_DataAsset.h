#pragma once

#include "CoreMinimal.h"
#include "BeatEnums.h"
#include "Engine/DataAsset.h"
#include "Boss_Harmyth_DataAsset.generated.h"

UENUM(BlueprintType)
enum class EBossBodyPart : uint8
{
    None       UMETA(DisplayName = "None"),
    Head       UMETA(DisplayName = "Head"),
    LeftHand   UMETA(DisplayName = "Left Hand"),
    RightHand  UMETA(DisplayName = "Right Hand"),
};

USTRUCT(BlueprintType)
struct FBossMeshDamageInfo {
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBossBodyPart PartType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanDealDamage;

    FBossMeshDamageInfo() : PartType(EBossBodyPart::None), bCanDealDamage(true) {}
    FBossMeshDamageInfo(EBossBodyPart InPart, bool bInCanDeal)
        : PartType(InPart), bCanDealDamage(bInCanDeal) {
    }
};

USTRUCT(BlueprintType)
struct FBossMove
{
    GENERATED_USTRUCT_BODY()

public:
    // === 비트 인덱스 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    int32 beatIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    FString moveName;

    // === 해당 비트 인덱스에 움직일 메쉬 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    EBossMeshType meshType;

    // === 보스의 움직임(이동 or 회전) ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    EBossMoveType moveType;

    // === X, Y, Z 위치 변수 현재 위치에서 추가로 움직임, Vector ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    FVector vectorValue;

    // === 현재 회전값에 더하는 수치 Rotator ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    FRotator rotationValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    float beatTime;

    UPROPERTY()
    float ElapsedTime = 0.0f;

    UPROPERTY()
    float TotalTime = 0.0f;

    UPROPERTY()
    FVector StartPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beat Data")
    bool bTriggeredEffect = false;

    UPROPERTY()
    USkeletalMeshComponent* TargetMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Move")
    TArray<FBossMeshDamageInfo> DamageInfos;
};

USTRUCT(BlueprintType)
struct FBossHit {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Boss HitTime")
    int32 HitStartBeat;

    UPROPERTY(EditAnywhere, Category = "Boss HitTime")
    int32 HitEndBeat;
};

USTRUCT(BlueprintType)
struct FBossDance {
    GENERATED_USTRUCT_BODY()

public:
    // 몇 비트마다 고개를 움직일 것인가
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Dance")
    int32 HeadDanceCycle = 4;

    // 얼마나 고개를 내밀 것인가 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Dance")
    float HeadMoveDistance = 20.f;

    // 고개를 내미는 데 걸리는 비트 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Dance")
    float HeadMoveTime = 1;

    // 고개를 원위치로 되돌리는 데 걸리는 비트 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Dance")
    float HeadReMoveTime = 1;
};

UCLASS(BlueprintType)
class UCISMA_API UBoss_Harmyth_DataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // === 보스의 Beat를 모아둔 배열 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Data")
    TArray<FBossMove> beatBossMove;

    UPROPERTY(EditAnywhere, Category = "Boss GroggyTime")

    float groggyBeatIndex;

    UPROPERTY(EditAnywhere, Category = "Boss GroggyTime")
    float groggyRunningBeat;

    UPROPERTY(EditAnywhere, Category = "Boss GroggyTime")
    float groggyDecayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss HitTime")
    FBossHit beatBossHit;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Dance")
    FBossDance beatBossDance;



    UPROPERTY(EditAnywhere, Category = "BossStage CheckTime")
    int32 CheckFailedBeat;


    UPROPERTY(EditAnywhere, Category = "Boss Stat")
    float maxHealth;

    UPROPERTY(EditAnywhere, Category = "Boss Stat")
    float damage;

    // 각 행동에서 DamageInfos를 안 채운 경우에 쓸 기본값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Default Damage")
    TArray<FBossMeshDamageInfo> DefaultDamageInfos;

    UBoss_Harmyth_DataAsset();
};
