// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Widget_AdjustJudgement.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_AdjustJudgement : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Init(class AStageManager* InputStageManager);
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);
	UFUNCTION(BlueprintCallable)
	void PlayRectangleAnim();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* BeatRectangleAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Beat, BlueprintReadWrite)
	int BeatIndex;

private:
	class AStageManager* StageManager;
	

	
};
