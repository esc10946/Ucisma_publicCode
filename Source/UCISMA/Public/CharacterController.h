// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API ACharacterController : public APlayerController
{
    GENERATED_BODY()

public:
    // Constructor
    ACharacterController();

protected:
    // BeginPlay 함수 선언 (APlayerController에서 오버라이드)
    virtual void BeginPlay() override;

public:
    // Tick 함수 선언
    virtual void Tick(float DeltaTime) override;
};