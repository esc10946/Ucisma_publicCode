// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor_Obstacles.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "BeatUnit/ActorComponent/UBeatRotatorComponent.h"
#include "Kismet/GameplayStatics.h"

ABeatUnitActor_Obstacles::ABeatUnitActor_Obstacles()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 스태틱 메시 컴포넌트 추가
    objectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
    RootComponent = objectMesh;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxCollision->SetupAttachment(RootComponent);

    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeatUnitActor_Obstacles::OnOverlapBegin);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);


}

void ABeatUnitActor_Obstacles::BeginPlay()
{
    Super::BeginPlay();

    // 🔹 이동 속도 설정
    if (bCanMove)
    {
        CurrentVelocity = moveDirection.GetSafeNormal() * moveSpeed;
    }

    if (!bCanDealDamage||!auraMaterial) return;
    // 🔹 대미지 이펙트 활성화 여부 설정
    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(auraMaterial, this);
    objectMesh->SetOverlayMaterial(DynamicMaterial);
}

void ABeatUnitActor_Obstacles::BeatUpdate(int QuaterIndex)
{
    Super::BeatUpdate(QuaterIndex);

    for (auto& Move : MoveArray)
    {
        if (Move.BeatIndex == CurrentBeatIndex &&
            Move.QuarterIndex == QuaterIndex)
        {
            CurrentVelocity = Move.MoveDirection.GetSafeNormal() * Move.MoveSpeed;
        }
    }
}

void ABeatUnitActor_Obstacles::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!GetIsLiving()) return;

    if (bCanMove)
    {
        FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
        SetActorLocation(NewLocation);  
    }
}

void ABeatUnitActor_Obstacles::KnockBack(AActor* player)
{
    if (player)
    {
        // 방향 계산
        FVector PushDirection = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector PushForce = PushDirection * 1000.0f; // 1000.0f는 강도 값
        PushForce.Z = 0;

        APlayerCharacter* Player = Cast<APlayerCharacter>(player);

        // 밀어내기
        Player->LaunchCharacter(PushForce, true, true);
        Player->Take_HP(-damageAmount, this);
    }
}

void ABeatUnitActor_Obstacles::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    if(OtherActor->ActorHasTag(FName("PurificationTarget"))) return;

    // 충돌한 오브젝트가 플레이어인지 확인
    if (Cast<APlayerCharacter>(OtherActor)&&bCanDealDamage)
    {
        KnockBack(OtherActor);
        return;  // 플레이어와 충돌 시 아무 행동도 하지 않음
    }

    // 충돌한 오브젝트가 다른 오브젝트일 경우

    if (OtherActor == PreviouslyBumpedActor || OtherActor->ActorHasTag(FName("Tag_Camera"))) return;
    
    //LOG_KHJ_SCREEN(3.0f, TEXT("오브젝트와 충돌! (%s)"), *OtherActor->GetName());

    switch (collisionResponse)
    {
    case ECollisionResponseType::Stop:
        bCanMove = false;
        CurrentVelocity = FVector::ZeroVector;
        //LOG_KHJ_SCREEN(3.0f, TEXT("오브젝트 정지!"));
        break;

    case ECollisionResponseType::Ignore:
        // LOG_KHJ_SCREEN(3.0f, TEXT("충돌 무시하고 이동 계속!"));
        break;

    case ECollisionResponseType::Reverse:
        CurrentVelocity *= -1.0f;
        //LOG_KHJ_SCREEN(3.0f, TEXT("반대 방향으로 이동!"));
        break;

    case ECollisionResponseType::Destroy:
        //LOG_KHJ_SCREEN(3.0f, TEXT("오브젝트 삭제!"));
        Destroy();
        break;
    }

    PreviouslyBumpedActor = OtherActor;
}

