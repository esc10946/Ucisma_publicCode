// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesign/TutorialStageDataAsset.h"

UTutorialStageDataAsset::UTutorialStageDataAsset()
{
    for (auto& TutorialData : TutorialDataSets)
    {
        FTutorialStepData& DataRef = TutorialData.Value;
        FTutorialUIVisible& UIRef = DataRef.UIVisible;

        const UEnum* EnumPtr = StaticEnum<EUIType>();
        if (!EnumPtr) continue;

        UMetaData* MetaData = EnumPtr->GetOutermost()->GetMetaData();
        if (!MetaData) continue;

        int32 EnumCount = EnumPtr->NumEnums();

        for (int32 i = 0; i < EnumCount; ++i)
        {
            FName EnumName = EnumPtr->GetNameByIndex(i);

            FString HiddenMetaKey = FString::Printf(TEXT("%s.Hidden"), *EnumName.ToString());
            FString SpacerMetaKey = FString::Printf(TEXT("%s.Spacer"), *EnumName.ToString());

            const FString HiddenMeta = MetaData->GetValue(EnumPtr, *HiddenMetaKey);
            const FString SpacerMeta = MetaData->GetValue(EnumPtr, *SpacerMetaKey);

            if (HiddenMeta.IsEmpty() && SpacerMeta.IsEmpty())
            {
                EUIType Key = static_cast<EUIType>(EnumPtr->GetValueByIndex(i));
                if (!UIRef.UIVisiblitys.Contains(Key))
                {
                    UIRef.UIVisiblitys.Add(Key, false); // 기본값 false로 초기화
                }
            }
        }
    }
}   

const FTutorialStepData UTutorialStageDataAsset::GetStepData(ETutorialStep Step)
{
    FTutorialStepData returnData;
    if (!TutorialDataSets.Contains(Step)) {
        return returnData;
    }
    returnData = *TutorialDataSets.Find(Step);

    return returnData;
}

const TMap<ETutorialStep, FTutorialStepData> UTutorialStageDataAsset::GetTutoralDataSets()
{
    return TMap<ETutorialStep, FTutorialStepData>();
}

const int UTutorialStageDataAsset::GetStepStartBeatIndex(ETutorialStep Step)
{
    if (!TutorialDataSets.Contains(Step)) return -1;

    if (const FTutorialStepData* Found = TutorialDataSets.Find(Step))
    {
        return Found->FStepBeatIndexs.StartBeatIndex;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetStepBeatIndex: Step %d not found!"), (int32)Step);
        return -1; // �Ǵ� ��ȿ�� �⺻��
    }
}
const int UTutorialStageDataAsset::GetStepEndBeatIndex(ETutorialStep Step)
{
    if (!TutorialDataSets.Contains(Step)) return -1;

    if (const FTutorialStepData* Found = TutorialDataSets.Find(Step))
    {
        return Found->FStepBeatIndexs.EndBeatIndex;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetStepBeatIndex: Step %d not found!"), (int32)Step);
        return -1; // �Ǵ� ��ȿ�� �⺻��
    }
}
const FTutorialUIVisible UTutorialStageDataAsset::GetUI_Tmap(ETutorialStep Step)
{
    if (!TutorialDataSets.Contains(Step)) return FTutorialUIVisible();

    if (const FTutorialStepData* Found = TutorialDataSets.Find(Step))
    {
        return Found->UIVisible;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetUI_Tmap: Step %d not found in map!"), (int32)Step);
        return FTutorialUIVisible();
    }
}

const FTutorialTextData UTutorialStageDataAsset::GetTextData(ETutorialStep Step)
{
    return FTutorialTextData();
}

const bool UTutorialStageDataAsset::GetNeedToLoop(ETutorialStep Step)
{
    return false;
}

bool FTutorialUIVisible::GetUiActive(EUIType type)
{
    if (UIVisiblitys.Contains(type)) {
        return UIVisiblitys[type];
    }
    return false;
}
