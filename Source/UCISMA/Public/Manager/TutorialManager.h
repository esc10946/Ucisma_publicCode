// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/CommonTypes.h"
#include "Components/ActorComponent.h"
#include "LevelDesign/TutorialStageDataAsset.h"
#include "TutorialManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTutorialTextRequested, const FString&, MainText, const FString&, SubText);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialUIActive, EUIType, PressedKey, bool, Active);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialKeyPressed, ETutorialKeyType, PressedKey);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialDashPressed, EJudgementResult, DashType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialAttackPressed, int32, AttackCount, int32, AttackCountStep8);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialStepChanged, ETutorialStep, CurrentStep);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialWarningAnimation);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCISMA_API UTutorialManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTutorialManager();

	void SetCurrentStep(ETutorialStep value);
	const ETutorialStep GetCurrentStep() { return currentStep; }
	void CanNextStep(bool NextStep) { bCanNextStep = NextStep; }

	UFUNCTION(BlueprintCallable)
	void Init(class AStageManager* InputStageManager);
	UFUNCTION(CallInEditor, Category = "Data")
	void LoadGameDataAsset();
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);
	UFUNCTION(BlueprintCallable)
	bool GoNextStep();
	UFUNCTION(BlueprintCallable)
	bool GoPreviousStep();
	UFUNCTION(BlueprintCallable)
	void ShowTutorialUI();
	void HideTutorial();
	void OnTutorialKeyInput(ETutorialKeyType Key);
	void OnTutorialDashInput(EJudgementResult dashjudge);
	void OnTutorialAttackInput();
	void OnTutorialHealthUp();
	void OnTutorialEnemySlayed(int SlayedEnemyCount);
	void OnTutorialConnectEnemy();
	void ActiveUserWidget();

	bool AreAllTutorialKeysPressed() const;
	bool AreAllTutorialDashesPressed() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialUIActive OnTutorialUIActive;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FTutorialTextRequested OnRequestTutorialText;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialKeyPressed OnTutorialKeyPressed;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialDashPressed OnTutorialDashPressed;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialAttackPressed OnTutorialAttackPressed;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialStepChanged OnTutorialStepChanged;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialWarningAnimation OnTutorialWarningAnimation;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "TutorialSpawnPoints")
	TArray<FVector> Points;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	UTutorialStageDataAsset* TutorialDataSet;

	//현재 단계에서 시작 index
	int32 currentStepStartIndex;
	//현재 단계에서 끝 index
	int32 currentStepEndIndex;
	//출력해야하는 튜토리얼 설명문
	FText mainText;
	//출력해야하는 튜토리얼 추가 설명문
	FText subText;

	//루프를 안하고 넘어가는 지
	bool bCanNextStep;
	int32 tutorialBeatIndex;
	int32 totalBeatIndex;

	UPROPERTY()
	ETutorialStep currentStep;

	class AStageManager* stageManager;
	TMap<ETutorialKeyType, bool> PressedKeys;
	TMap<EJudgementResult, bool> PressedDashes;
	int8 attackCount;
	int8 attackCountStep8;

	UPROPERTY()
	class AEnemyBase* TutorialEnemy1;
	UPROPERTY()
	class AEnemyBase* TutorialEnemy2;
	UPROPERTY()
	class ABeatUnitActor_Laser* TutorialLaser1;
	UPROPERTY()
	class ABeatUnitActor_Laser* TutorialLaser2;
	UPROPERTY()
	class ABeatUnitActor_Obstacles* TutorialObstacles;

	TSubclassOf<class AEnemyBase> EnemyClass;
	TSubclassOf<class ABeatUnitActor_Laser> LaserClass;
	TSubclassOf<class ABeatUnitActor_Obstacles> ObstacleClass;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
