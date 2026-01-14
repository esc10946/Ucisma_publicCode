// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MainMenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"



void AMainMenuGameMode::ResetWidgets()
{
	for (int32 i = 0; i < StaticEnum<EMenu>()->NumEnums() - 1; i++)
	{
		if (MenuWidgets[static_cast<EMenu>(i)])
		{
			MenuWidgets[static_cast<EMenu>(i)]->RemoveFromParent();
		}
	}
}

UUserWidget* AMainMenuGameMode::OpenMenu(EMenu Menu)
{
	ResetWidgets();
	if (!MenuWidgets[Menu])
	{
		LOG_DEBUG(Warning, TEXT("Menu Widgets didn't init"));
		return nullptr;
	}
	MenuWidgets[Menu]->AddToViewport();

	return MenuWidgets[Menu];
}

