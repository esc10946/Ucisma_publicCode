// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "AIController.h"
#include "Enemy_AIController.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API AEnemy_AIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_AIController();

protected:
	virtual void BeginPlay();

public:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* EnemyBehaviorTree;
};
