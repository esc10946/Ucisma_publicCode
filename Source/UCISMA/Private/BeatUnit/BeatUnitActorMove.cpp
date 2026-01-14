// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActorMove.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ABeatUnitActorMove::ABeatUnitActorMove()
{
    bIsCamInitialized = false;
}

void ABeatUnitActorMove::Tick(float DeltaTime)
{
	if (!GetIsLiving()) return;

	Super::Tick(DeltaTime);

    ChaseCamera();
}


void ABeatUnitActorMove::ChaseCamera()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    
    if (!PC) {
        LOG_KHJ(Error, TEXT("NO Player Controller!"));
    }

    if (!bIsCamInitialized)
    {
        InitCameraDistance(PC);
    }
    else {
        //ī�޶� ��ġ �ľ�
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

        // ������ ��ġ ������Ʈ: ī�޶� ��ġ���� ���� �Ÿ� ����
        FVector NewLaserLocation = CameraLocation + CameraDistance;
        SetActorLocation(NewLaserLocation);
    }
}

void ABeatUnitActorMove::InitCameraDistance(APlayerController* PC)
{
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
    CameraDistance = GetActorLocation() - CameraLocation;

    bIsCamInitialized = true;
}
