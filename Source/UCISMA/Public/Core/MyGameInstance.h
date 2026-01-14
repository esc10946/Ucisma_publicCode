// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API UMyGameInstance : public UGameInstance
{
	
	GENERATED_BODY()

public:
	/*** 함수 ***/
	UFUNCTION(BlueprintCallable)
	void SetBgmVolume(int Volume);
	UFUNCTION(BlueprintCallable)
	void SetSfxVolume(int Volume);
	UFUNCTION(BlueprintCallable)
	void SetUIVolume(int Volume);
	UFUNCTION(BlueprintCallable)
	void SetMetronomeVolume(int Volume);
	UFUNCTION(BlueprintCallable)
	void SetInputOffset(float Value);
	UFUNCTION(BlueprintCallable)
	void SetGraphicQuality(int Value);
	UFUNCTION(BlueprintCallable)
	void ResetStageRecord();
	UFUNCTION(BlueprintCallable)
	FText GetRandomTips();

	/********************/
	/********로딩********/
	/********************/

	UFUNCTION(BlueprintCallable)
	void LoadingLevel(FName TargetLevelName);
	UFUNCTION(BlueprintCallable)
	void LoadStageAsset(EStageIndex StageIndex);
	UFUNCTION(BlueprintCallable)
	void EndLoadingStageAsset();
	UFUNCTION(BlueprintCallable)
	void ShowLoadingUI();
	UFUNCTION(BlueprintCallable)
	void RemoveLoadingUI();
	UFUNCTION(BlueprintCallable)
	void ResearchLoadingTargetAsset(TArray<TSoftObjectPtr<UStaticMesh>>& ReturnStaticMeshSoftPtr, TArray<TSoftObjectPtr<USkeletalMesh>>& ReturnSkeletalMeshSoftPtr, TArray<FSoftObjectPath>& ReturnPathList);
	UFUNCTION(BlueprintCallable)
	const bool& IsAssetLoadingComplete() { return bIsAssetLoadingComplete; }

	UFUNCTION(BlueprintCallable)
	const FName& GetTargetLevelName() { return TargetLevelName; }
	FStreamableManager& GetStreamableManager() { return StreamableManager; }
	


	/*** 변수 ***/
	//판정과 음악의 출력의 길이를 조절하여 싱크를 맞추는 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	float InputOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int BgmVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int SfxVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int UIVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int MetronomeVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting)
	int GraphicQuality;
	//게임에서 사용하는 세이브 슬롯의 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Save)
	FString SaveSlotName;
	//로딩을 할 레벨의 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loading)
	FName LoadingLevelName;
	//로딩 UI의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loading)
	TSubclassOf<class UUserWidget> LoadingUIWidgetClass;
	//로딩 UI의 인스턴스
	class UUserWidget* LoadingUIWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Platform, meta = (AllowPrivateAccess = "true"))
	bool bIsPC;
	//다음 메인화면에 갔을 때 로드되는 화면
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenu InitMainMenu;
private:
	FName TargetLevelName;

	FStreamableManager StreamableManager;
	TArray<FSoftObjectPath> LoadingTargetSoftObjectPaths;
	TArray<TSoftObjectPtr<UStaticMesh>> LoadingTargetStaticMeshAssetSoftPtr;
	TArray<TSoftObjectPtr<USkeletalMesh>> LoadingTargetSkeletalMeshAssetSoftPtr;
	TArray<UObject*> LoadedAssets;

	bool bIsAssetLoadingComplete = false;
	
};
