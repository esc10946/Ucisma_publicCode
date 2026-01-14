// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_GameplayHUD.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_GameplayHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(class AStageManager* InputStageManager);
	UPROPERTY(meta = (BindWidget))
	class UWidget_BeatBar* BeatBar;
	UPROPERTY(meta = (BindWidget))
	class UWidget_SongProgressBar* SongProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UWidget_Score* Score;
	UPROPERTY(meta = (BindWidget))
	class UWidget_Combo* Combo;
	UPROPERTY(meta = (BindWidget))
	class UWiget_State* State;
};
