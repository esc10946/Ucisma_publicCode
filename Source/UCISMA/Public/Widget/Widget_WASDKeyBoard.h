// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/CommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "Widget_WASDKeyBoard.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_WASDKeyBoard : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init();
	void ChangeKeyBoardImage(ETutorialKeyType PressedKey);
private:
	TMap<ETutorialKeyType, class UImage*> KeyBoardImages;
	class UImage* W_Right;
	class UImage* A_Right;
	class UImage* S_Right;
	class UImage* D_Right;
};
