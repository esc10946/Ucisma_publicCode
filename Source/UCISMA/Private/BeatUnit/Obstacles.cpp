// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/Obstacles.h"
#include "PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "UObject/ConstructorHelpers.h"

AObstacles::AObstacles()
{
    PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(RootComponent);

    Damage = 1;
    IsLiving = false;
}

void AObstacles::BeginPlay()
{
    Super::BeginPlay();
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AObstacles::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    IsLiving = GetIsLiving();
}

void AObstacles::KnockBack()
{
    if (PlayerCharacter)
    {
        // 방향 계산
        FVector PushDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector PushForce = PushDirection * 1000.0f; // 1000.0f는 강도 값
        PushForce.Z = 0;

        // 밀어내기
        PlayerCharacter->LaunchCharacter(PushForce, true, true);
        DamageApply();
    }
}

void AObstacles::DamageApply()
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerCharacter);
    Player->Take_HP(-Damage, this);
}