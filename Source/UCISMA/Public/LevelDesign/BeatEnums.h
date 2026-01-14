// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatEnums.generated.h"

// === 보스가 움직일 메쉬 (추가적으로 전체도 넣어야 할듯) ===
UENUM(BlueprintType)
enum class EBossMeshType : uint8
{
    LeftHand UMETA(DisplayName = "Left Hand"),
    RightHand UMETA(DisplayName = "Right Hand"),
    BothHands UMETA(DisplayName = "Both Hands"),
    Head UMETA(DisplayName = "Head"),
    Body UMETA(DisplayName = "Body")
};

// === 보스가 할 행동(회전 or 이동) ===
UENUM(BlueprintType)
enum class EBossMoveType : uint8
{
    Vector UMETA(DisplayName = "Move"),
    Rotate UMETA(DisplayName = "Rotate"),
    Teleport UMETA(DisplayName = "Teleport"),
    TeleportRota UMETA(DisplayName = "TeleportRota")
};
