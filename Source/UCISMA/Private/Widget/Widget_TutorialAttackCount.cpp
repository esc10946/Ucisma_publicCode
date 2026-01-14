// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_TutorialAttackCount.h"
#include "Components/Image.h"

void UWidget_TutorialAttackCount::Init(int32 index)
{
	MaxIndex = index;
	for (int32 i = 1; i <= MaxIndex; ++i)
	{
		FString WidgetName = FString::Printf(TEXT("CheckBoxImage_ON%d"), i);
		if (UWidget* FoundWidget = GetWidgetFromName(FName(*WidgetName)))
		{
			if (UImage* Image = Cast<UImage>(FoundWidget))
			{
				AttackCountCheckBox.Add(Image);
				Image->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				LOG_KHJ(Warning, TEXT("Widget %s is not a UImage!"), *WidgetName);
			}
		}
		else
		{
			LOG_KHJ(Warning, TEXT("Widget %s not found!"), *WidgetName);
		}
	}
	ChangeAttackCheckBox(0);
	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_TutorialAttackCount::ChangeAttackCheckBox(int8 value)
{
	int index = FMath::Clamp(value, 0, MaxIndex);

	for (int8 i = 0; i < index; i++) {
		if(AttackCountCheckBox[i]){
			AttackCountCheckBox[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			LOG_KHJ(Warning, TEXT("AttackCheckBox %d not found!"), i);
		}
	}
}
