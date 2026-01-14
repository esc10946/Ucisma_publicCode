// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "UCISMA/UCISMA.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Dash.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UBTService_Dash : public UBTService
{
	GENERATED_BODY()

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
