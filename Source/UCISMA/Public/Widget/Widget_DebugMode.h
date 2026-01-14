// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DebugMode.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_DebugMode : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(class AStageManager* InputStageManager);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDebugMode();

	UPROPERTY(BlueprintReadOnly)
	class AStageManager* StageManager;
private:
	
};
