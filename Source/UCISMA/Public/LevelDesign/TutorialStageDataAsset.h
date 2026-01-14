// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "System/CommonTypes.h"
#include "TutorialStageDataAsset.generated.h"

/**
 * 
 */
//Ʃ�丮�󿡼� ������ Text��
USTRUCT(BlueprintType)
struct FTutorialTextData
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText MainText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText SubText;
};

//Ʃ�丮�󿡼� ������ UI�� ����
USTRUCT(BlueprintType)
struct FTutorialUIVisible {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TMap<EUIType, bool> UIVisiblitys;

    bool GetUiActive(EUIType type);
};

//�� �ܰ谡 ���Ʈ ���� ����Ǵ���
USTRUCT(BlueprintType)
struct FTutorialStepBeatIndex {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeatIndex|Start")
    int32 StartBeatIndex;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeatIndex|End")
    int32 EndBeatIndex;
};

USTRUCT(BlueprintType)
struct FTutorialStepData {
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere)
    FTutorialStepBeatIndex FStepBeatIndexs;

    UPROPERTY(EditAnywhere)
    FTutorialUIVisible UIVisible;

    UPROPERTY(EditAnywhere)
    FTutorialTextData TextData;

    UPROPERTY(EditAnywhere)
    bool bNeedToLoop = false;
};



UCLASS()
class UCISMA_API UTutorialStageDataAsset : public UDataAsset
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TMap<ETutorialStep, FTutorialStepData> TutorialDataSets;
public:
    UTutorialStageDataAsset();

    UFUNCTION()
    const FTutorialStepData GetStepData(ETutorialStep Step);
    const TMap<ETutorialStep, FTutorialStepData> GetTutoralDataSets();
    const int GetStepStartBeatIndex(ETutorialStep Step);
    const int GetStepEndBeatIndex(ETutorialStep Step);
    const FTutorialUIVisible GetUI_Tmap(ETutorialStep Step);
    const FTutorialTextData GetTextData(ETutorialStep Step);
    const bool GetNeedToLoop(ETutorialStep Step);
};
