// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/BeatUnitActor.h"
#include "BeatUnitActorMove.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API ABeatUnitActorMove : public ABeatUnitActor
{
	GENERATED_BODY()
public:
    ABeatUnitActorMove();

protected:

    UFUNCTION(BlueprintCallable)
    void ChaseCamera();

    UFUNCTION(BlueprintCallable)
    void InitCameraDistance(APlayerController* PC);

public:
    virtual void Tick(float DeltaTime) override;

private:
    FVector CameraLocation;
    FRotator CameraRotation;

    FVector CameraDistance;

    bool bIsCamInitialized;
};
