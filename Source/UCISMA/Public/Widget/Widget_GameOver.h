// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "System/CommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "Widget_GameOver.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_GameOver : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void Init(class AStageManager* InputStageManager);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGameOverUI(EGameOverType GameOverType);
	//변수
	UPROPERTY(BlueprintReadWrite, Category = "Design Settings", EditAnywhere)
	TMap<EGameOverType, UTexture2D*> GameOverTextures;
	UPROPERTY(BlueprintReadOnly)
	class AStageManager* StageManager;
	
};
