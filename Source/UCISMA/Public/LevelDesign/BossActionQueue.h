// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossActionQueue.generated.h"

/**
 * 
 */
class UCISMA_API BossActionQueue
{

public:
	BossActionQueue();
	~BossActionQueue();
};

USTRUCT(Atomic, BlueprintType)
struct FBossActionStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float Speed;

	UPROPERTY()
	UStaticMeshComponent* Mesh;
};