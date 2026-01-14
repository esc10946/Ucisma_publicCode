// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "UCISMA/UCISMA.h"
#include "GameFramework/SaveGame.h"
#include "Enum_CamDirection.h"
#include "SaveInstance.generated.h"

/**
 *
 */
UCLASS()
class UCISMA_API USaveInstance : public USaveGame
{
    GENERATED_BODY()
public:
    // 타일 위치 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
    TArray<FVector> Saved_TilePositions;

    // 타일 방향 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
    TArray<ECamDirection> Saved_TileDirections;

    // 현재 인덱스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
    int32 Saved_CurrentIndex;

};