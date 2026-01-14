// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/CommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "Widget_TutorialDashCheckBox.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_TutorialDashCheckBox : public UUserWidget
{
	GENERATED_BODY()
	
	TMap<EJudgementResult, class UImage*> CheckBoxes;
public:
	UFUNCTION(BlueprintCallable)
	void Init(class AStageManager* InputStageManager);

	void ChangeDashCheckBox(EJudgementResult type);
};
