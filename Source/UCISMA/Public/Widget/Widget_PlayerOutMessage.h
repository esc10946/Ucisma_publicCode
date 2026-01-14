// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerOutMessage.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_PlayerOutMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PrintPlayerOutMessage(float AnimSpeed);
	
private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PrintMessage;
};
