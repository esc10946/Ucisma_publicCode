// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyGameInstance.h"
#include "Manager/StageManager.h"
#include "Kismet/GameplayStatics.h"
#include "System/SaveData.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "GameFramework/GameUserSettings.h"
#include "EngineUtils.h"
#include "LevelDesign/GameDataAsset.h"

#define MIN_VOLUME_SIZE 0
#define MAX_VOLUME_SIZE 10 
#define MIN_GRAPHIC_QUALITY 0
#define MAX_GRAPHIC_QUALITY 4
#define VOLUME_NORMALIZATION 10.0f


void UMyGameInstance::SetBgmVolume(int Volume)
{
	//예외 처리
	Volume = FMath::Max(MIN_VOLUME_SIZE, Volume);
	Volume = FMath::Min(MAX_VOLUME_SIZE, Volume);
	//변수 선언
	USoundMix* BgmSoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/SoundClass/BgmSoundClassMix.BgmSoundClassMix"));
	USoundClass* BgmSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SoundClass/BgmSoundClass.BgmSoundClass"));
	if (!(BgmSoundMix && BgmSoundClass))
	{
		LOG_DEBUG(Error, TEXT("BgmSoundMix or BgmSoundClass Load Failed"));
		return;
	}
	USaveData* SaveData;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}

	//볼륨 값 조절
	BgmVolume = Volume;
	//볼륨 값을 사운드 믹스에 적용
	UGameplayStatics::SetSoundMixClassOverride(this, BgmSoundMix, BgmSoundClass, static_cast<int>(BgmVolume) / VOLUME_NORMALIZATION, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, BgmSoundMix);
	//설정한 볼륨 값을 세이브에 저장
	SaveData->SaveBgmVolume(BgmVolume);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::SetSfxVolume(int Volume)
{
	//예외 처리
	Volume = FMath::Max(MIN_VOLUME_SIZE, Volume);
	Volume = FMath::Min(MAX_VOLUME_SIZE, Volume);
	//변수 선언
	USoundMix* SfxSoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/SoundClass/SfxSoundClassMix.SfxSoundClassMix"));
	USoundClass* SfxSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SoundClass/SfxSoundClass.SfxSoundClass"));
	if (!(SfxSoundMix && SfxSoundClass))
	{
		LOG_DEBUG(Error, TEXT("SfxSoundMix or SfxSoundClass Load Failed"));
		return;
	}
	USaveData* SaveData;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}

	//볼륨 값 조절
	SfxVolume = Volume;
	//볼륨 값을 사운드 믹스에 적용
	UGameplayStatics::SetSoundMixClassOverride(this, SfxSoundMix, SfxSoundClass, static_cast<int>(SfxVolume) / VOLUME_NORMALIZATION, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, SfxSoundMix);
	//설정한 볼륨 값을 세이브에 저장
	SaveData->SaveSfxVolume(SfxVolume);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::SetUIVolume(int Volume)
{
	//예외 처리
	Volume = FMath::Max(MIN_VOLUME_SIZE, Volume);
	Volume = FMath::Min(MAX_VOLUME_SIZE, Volume);
	//변수 선언
	USoundMix* UISoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/SoundClass/UISoundClassMix.UISoundClassMix"));
	USoundClass* UISoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SoundClass/UISoundClass.UISoundClass"));
	if (!(UISoundMix && UISoundClass))
	{
		LOG_DEBUG(Error, TEXT("UISoundMix or UISoundClass Load Failed"));
		return;
	}
	USaveData* SaveData;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}

	//볼륨 값 조절
	UIVolume = Volume;
	//볼륨 값을 사운드 믹스에 적용
	UGameplayStatics::SetSoundMixClassOverride(this, UISoundMix, UISoundClass, static_cast<int>(UIVolume) / VOLUME_NORMALIZATION, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, UISoundMix);
	//설정한 볼륨 값을 세이브에 저장
	SaveData->SaveUIVolume(UIVolume);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::SetMetronomeVolume(int Volume)
{
	//예외 처리
	Volume = FMath::Max(MIN_VOLUME_SIZE, Volume);
	Volume = FMath::Min(MAX_VOLUME_SIZE, Volume);
	//변수 선언
	USoundMix* SoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/Audio/SoundClass/MetronomeSoundClassMix.MetronomeSoundClassMix"));
	USoundClass* SoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SoundClass/MetronomeSoundClass.MetronomeSoundClass"));
	if (!(SoundMix && SoundClass))
	{
		LOG_DEBUG(Error, TEXT("MetronomeSoundMix or MetronomeSoundClass Load Failed"));
		return;
	}
	USaveData* SaveData;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}

	//볼륨 값 조절
	MetronomeVolume = Volume;
	//볼륨 값을 사운드 믹스에 적용
	UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, SoundClass, static_cast<int>(MetronomeVolume) / VOLUME_NORMALIZATION, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, SoundMix);
	//설정한 볼륨 값을 세이브에 저장
	SaveData->SaveMetronomeVolume(MetronomeVolume);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::SetInputOffset(float Value)
{
	USaveData* SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	InputOffset = Value;
	SaveData->SaveInputOffset(InputOffset);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::SetGraphicQuality(int Value)
{
	//예외 처리
	Value = FMath::Max(MIN_GRAPHIC_QUALITY, Value);
	Value = FMath::Min(MAX_GRAPHIC_QUALITY, Value);
	//세이브 데이터 불러오기
	USaveData* SaveData;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}

	//그래픽 값 조절
	GraphicQuality = Value;
	//그래픽 값을 게임에 조절
	UGameUserSettings* Setting = GEngine->GetGameUserSettings();
	if (Setting)
	{
		Setting->SetOverallScalabilityLevel(GraphicQuality);
		Setting->ApplySettings(false);
	}
	//설정한 그래픽 값을 세이브에 저장
	SaveData->SaveGraphicQuality(GraphicQuality);
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::ResetStageRecord()
{
	USaveData* SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

	SaveData->ResetAllStageRecord();
	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UMyGameInstance::LoadingLevel(FName LevelName)
{
	TargetLevelName = LevelName;
	UGameplayStatics::OpenLevel(this, LoadingLevelName);
}

void UMyGameInstance::LoadStageAsset(EStageIndex StageIndex)
{
	ShowLoadingUI();
	LoadingTargetStaticMeshAssetSoftPtr.Empty();
	LoadingTargetSkeletalMeshAssetSoftPtr.Empty();
	LoadingTargetSoftObjectPaths.Empty();
	ResearchLoadingTargetAsset(LoadingTargetStaticMeshAssetSoftPtr, LoadingTargetSkeletalMeshAssetSoftPtr, LoadingTargetSoftObjectPaths);
	StreamableManager.RequestAsyncLoad(LoadingTargetSoftObjectPaths, FStreamableDelegate::CreateUObject(this, &UMyGameInstance::EndLoadingStageAsset));
}

void UMyGameInstance::EndLoadingStageAsset()
{
	int temp = 0;
	
	bIsAssetLoadingComplete = true;
}

void UMyGameInstance::ResearchLoadingTargetAsset(TArray<TSoftObjectPtr<UStaticMesh>>& ReturnStaticMeshSoftPtr, TArray<TSoftObjectPtr<USkeletalMesh>>& ReturnSkeletalMeshSoftPtr ,TArray<FSoftObjectPath>& ReturnPathList)
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//스태틱 매시 탐색
		UStaticMeshComponent* StaticMeshComponent = ActorItr->FindComponentByClass<UStaticMeshComponent>();
		if (StaticMeshComponent)
		{
			UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
			if (StaticMesh)
			{
				ReturnStaticMeshSoftPtr.Add(StaticMesh);
				ReturnPathList.AddUnique(ReturnStaticMeshSoftPtr[ReturnStaticMeshSoftPtr.Num() - 1].ToSoftObjectPath());
			}
		}
		USkeletalMeshComponent* SkeletalMeshComponent = ActorItr->FindComponentByClass<USkeletalMeshComponent>();
		if (SkeletalMeshComponent)
		{
			USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
			if (SkeletalMesh)
			{
				ReturnSkeletalMeshSoftPtr.Add(SkeletalMesh);
				ReturnPathList.AddUnique(ReturnSkeletalMeshSoftPtr[ReturnSkeletalMeshSoftPtr.Num() - 1].ToSoftObjectPath());
			}
		}
	}
	
	//테스트용
	for (const TSoftObjectPtr<UStaticMesh>& TargetSoftObjectPtr : ReturnStaticMeshSoftPtr)
	{
		//LOG_PSJ(Display, TEXT("%s - IsLoad? : %s"), *(TargetSoftObjectPtr.Get()->GetName()), (TargetSoftObjectPtr.IsValid() ? TEXT("True") : TEXT("False")));
	}
	for (const TSoftObjectPtr<USkeletalMesh>& TargetSoftObjectPtr : ReturnSkeletalMeshSoftPtr)
	{
		//LOG_PSJ(Display, TEXT("%s - IsLoad? : %s"), *(TargetSoftObjectPtr.Get()->GetName()), (TargetSoftObjectPtr.IsValid() ? TEXT("True") : TEXT("False")));
	}
}

void UMyGameInstance::ShowLoadingUI()
{
	LoadingUIWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LoadingUIWidgetClass);
	if (LoadingUIWidgetInstance)
	{
		LoadingUIWidgetInstance->AddToViewport(100);
	}
}

void UMyGameInstance::RemoveLoadingUI()
{
	if (LoadingUIWidgetInstance)
	{
		LoadingUIWidgetInstance->RemoveFromParent();
	}
}

FText UMyGameInstance::GetRandomTips()
{
	UGameDataAsset* GameDataAsset = LoadObject<UGameDataAsset>(nullptr, TEXT("/Game/Data/GameDataAsset.GameDataAsset"));

	TArray<FText> Tips = GameDataAsset->GetTips();

	int32 RandomIndex = FMath::RandRange(0, Tips.Num() - 1);

	return Tips[RandomIndex];
}