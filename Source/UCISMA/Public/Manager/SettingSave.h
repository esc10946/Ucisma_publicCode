// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/SaveGame.h"
#include "SettingSave.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API USettingSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	float InputOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int BgmVolumeSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int SfxVolumeSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int GraphicQuality;
private:
	USettingSave();
};
