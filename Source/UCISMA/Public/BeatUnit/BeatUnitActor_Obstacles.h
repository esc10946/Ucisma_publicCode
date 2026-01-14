// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "BeatUnit/BeatUnitActor.h"
#include "Components/BoxComponent.h"
#include "BeatUnitActor_Obstacles.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMoveInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BeatIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 QuarterIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector MoveDirection;
};

UENUM(BlueprintType)
enum class ECollisionResponseType : uint8
{
    Stop UMETA(DisplayName = "멈추기"),
    Ignore UMETA(DisplayName = "무시하기"),
    Reverse UMETA(DisplayName = "반대 방향으로 움직이기"),
    Destroy UMETA(DisplayName = "사라지기")
};

UCLASS()
class UCISMA_API ABeatUnitActor_Obstacles : public ABeatUnitActor
{
	GENERATED_BODY()
public:
    ABeatUnitActor_Obstacles();

protected:
    virtual void BeginPlay() override;
    virtual void BeatUpdate(int QuaterIndex) override;

public:
    virtual void Tick(float DeltaTime) override;
    int GetEndBeatIndex() { return EndBeatIndex; };
    void SetEndBeatIndex(int32 value) {
        this->EndBeatIndex = value;
    }
    // ======================= [ 디테일 창에서 설정 가능하도록 UPROPERTY 추가 ] =======================

    // 🔹 오브젝트가 대미지를 주는 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings")
    bool bCanDealDamage = false;

    // 🔹 대미지 값 (bCanDealDamage가 true일 때 적용됨)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings", meta = (EditCondition = "bCanDealDamage"))
    float damageAmount = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings", meta = (EditCondition = "bCanDealDamage"))
    UMaterialInstance * auraMaterial;

    // 🔹 오브젝트 이동 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings")
    bool bCanMove = false;

    // 🔹 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings", meta = (EditCondition = "bCanMove"))
    float moveSpeed = 200.0f;

    // 🔹 이동 방향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings", meta = (EditCondition = "bCanMove"))
    FVector moveDirection = FVector(1.0f, 0.0f, 0.0f);

    // 🔹 충돌 시 행동 (멈추기, 무시하기, 반대 방향, 삭제)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings")
    ECollisionResponseType collisionResponse = ECollisionResponseType::Stop;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void KnockBack(AActor* player);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design Settings|MoveArray")
    TArray<FMoveInfo> MoveArray;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* objectMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UBoxComponent* BoxCollision;
private:

    AActor* PreviouslyBumpedActor;

    // 🔹 이동을 위한 변수
    FVector CurrentVelocity;

    // 🔹 충돌 처리 함수
    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
