// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_WASDKeyBoard.h"
#include "Components/Image.h"

void UWidget_WASDKeyBoard::Init()
{
	W_Right = Cast<UImage>(GetWidgetFromName(TEXT("W_Right")));
	A_Right = Cast<UImage>(GetWidgetFromName(TEXT("A_Right")));
	S_Right = Cast<UImage>(GetWidgetFromName(TEXT("S_Right")));
	D_Right = Cast<UImage>(GetWidgetFromName(TEXT("D_Right")));

	KeyBoardImages.Emplace(ETutorialKeyType::W, W_Right);
	KeyBoardImages.Emplace(ETutorialKeyType::A, A_Right);
	KeyBoardImages.Emplace(ETutorialKeyType::S, S_Right);
	KeyBoardImages.Emplace(ETutorialKeyType::D, D_Right);

	for (uint8 i = 0; i <= static_cast<uint8>(ETutorialKeyType::D); ++i)
	{
		ETutorialKeyType Key = static_cast<ETutorialKeyType>(i);
		KeyBoardImages[Key]->SetVisibility(ESlateVisibility::Hidden);
	}
	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_WASDKeyBoard::ChangeKeyBoardImage(ETutorialKeyType PressedKey)
{
	if (KeyBoardImages.Contains(PressedKey)) {
		KeyBoardImages[PressedKey]->SetVisibility(ESlateVisibility::Visible);
	}
}