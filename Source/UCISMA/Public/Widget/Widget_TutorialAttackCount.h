// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_TutorialAttackCount.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_TutorialAttackCount : public UUserWidget
{
	GENERATED_BODY()

	int32 MaxIndex;
	TArray<class UImage*> AttackCountCheckBox;
public:
	UFUNCTION(BlueprintCallable)
	void Init(int32 MaxIndex);

	void ChangeAttackCheckBox(int8 value);
};
