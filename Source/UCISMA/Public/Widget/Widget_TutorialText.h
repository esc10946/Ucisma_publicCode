// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/CommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "Widget_TutorialText.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_TutorialText : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	TMap<ETutorialStep, FText> TutorialTexts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	class UTextBlock* mainScript;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	class UTextBlock* subScript;
	class AStageManager* stageManager;
	class UTutorialManager* TutorialManagerComp;

	class UWidget_WASDKeyBoard* WidgetWASD;
	class UWidget_TutorialAttackCount* WidgetAttackCountBox;
	class UWidget_TutorialAttackCount* WidgetAttackCountBoxStep8;
	class UWidget_TutorialDashCheckBox* WidgetDashCountBox;

	class UImage* songProcessBarArrow;
	class UImage* attackNodeArrow;
	class UImage* dashNodeArrow;
	class UImage* beatBarArrow;
	class UImage* cursorArrow;

	UFUNCTION(BlueprintCallable)
	void Init(class AStageManager* InputStageManager);
	UFUNCTION(BlueprintCallable)
	void OutputTutorialText(const FText& mainText, const FText& subText);
	UFUNCTION(BlueprintCallable)
	void OnPressedKeyRight(ETutorialKeyType PressedKey);
	void InitArrowImages();

	void VisibleArrowImage(ETutorialStep type);
	void HiddenArrowImage();

	UFUNCTION()
	void ChangeDashCheckBox(EJudgementResult type);
	UFUNCTION()
	void ChangeAttackCheckBox(int32 count, int32 attackCount);
	UFUNCTION()
	void ChangeStep(ETutorialStep Step);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayanimationWarning();
	UFUNCTION(BlueprintImplementableEvent)
	void ChaseCusorStart();
private:
	TMap<ETutorialStep, TArray<UImage*>> ArrowImages;
	TMap<ETutorialStep, UUserWidget*> TutorialStepWidgets;
};
