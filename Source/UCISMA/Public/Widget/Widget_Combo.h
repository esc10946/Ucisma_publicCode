// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Combo.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_Combo : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(class AStageManager* InputStageManager);
	void UpdateCombo(int Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ComboUpAnim;

private:
	class AStageManager* StageManager;
	class UTextBlock* ComboText;
	
	
};
