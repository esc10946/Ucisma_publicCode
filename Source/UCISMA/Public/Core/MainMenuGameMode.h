// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "System/CommonTypes.h"
#include "MainMenuGameMode.generated.h"




UCLASS()
class UCISMA_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<EMenu, UUserWidget*> MenuWidgets;

	//모든 메뉴 위젯 초기화
	UFUNCTION(BlueprintCallable)
	void ResetWidgets();

	//특정 메뉴를 출력
	UFUNCTION(BlueprintCallable)
	UUserWidget* OpenMenu(EMenu Menu);
	
};
