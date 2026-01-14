// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyDataTable.h"
#include "BeatUnit/BeatUnitCharacter_Enemy.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "LevelDesign/Enum_CamDirection.h"
#include "PlayerCharacter.h"
#include "System/CommonTypes.h"
#include "LevelDesign/Camera_Map.h"
#include "EnemyBase.generated.h"
/**
 *
 */
class ACamera_Map;

UCLASS()
class UCISMA_API AEnemyBase : public ABeatUnitCharacter_Enemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	virtual void ActiveUnit() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//해당 유닛을 비활성화 하는 함수
	//virtual void DeactiveUnit() override;
	//해당 유닛을 활성화 하는 함수

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* EnemyDataTable;

	UPROPERTY(VisibleAnywhere, Category = "StatSystem")
	class UEnemyStatComp* EnemyStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "Attack")
	bool CanDash;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EnemyData")
	bool HasDashAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EnemyData")
	EApplyDamageType ApplyDamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* SkelMeshComp;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FVector SkelMeshSize = FVector(0.1f, 0.1f, 0.1f);

	UPROPERTY(EditAnywhere, Category = "Design Settings")
	int DataIndex = 1;

	UPROPERTY(EditAnywhere, Category = "Design Settings")
	ECommonEnemyType EnemyType;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FVector MeshLocation = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FRotator MeshRotation = FRotator(0.f, -90.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TArray<UNiagaraSystem*> NiagaraSystemAssets;

	//적 사망 후 필드에 남아있는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Purification_time = 1.5f;

	UMaterialInstanceDynamic* fade_material;

	class AStageManager* StageManager;
	class UTutorialManager* TutorialManager;
	////////////////////함수////////////////////////
	//기능 함수
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void Charge();

	//데이터 함수
	FEnemyDataStruct* GetEnemyData();

	//데이터 초기화 함수
	void LoadEnemyData();

	void InitEnemyData();

	//BeatUnit함수
	virtual void BeatUpdate(int QuaterIndex) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateHealthBar();

	virtual void TakeDamagePlayer(float Damage, EJudgementResult Judgement, AActor* DamageCauser) override;

	void OnEnemyHitFx(int index) override;

	bool IsAvailableForAttack(EAttackType AttackType);

	bool IsInView();

	void SetbInView(bool bIsInView);
	void SetCommonEnemyType(ECommonEnemyType NewEnemyType);
	//Get
	const UEnemyStatComp* GetEnemyStat() { return EnemyStat; }

protected:
	// === 대시 관련 변수 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDistance = 0;  // 대시 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDuration = 1.0f;  // 대시 지속 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	bool HasDashFx = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	UNiagaraSystem* DashFx;
	UNiagaraComponent* DashFxNiagaraComponent; 	//대쉬 나이아가라컴포넌트
	float Adjust_Dash_Location = 0.3f;

	bool bIsDashing = false;

	float DashTimeElapsed = 0.0f;

	FVector StartLocation; //시작위치
	FVector EndLocation; //도착위치

	void AttackPlayer(APlayerCharacter* Player);
private:

	//EnemyStat관련 변수
	UPROPERTY(VisibleAnywhere, Category = "StatSystem")
	FName ID;
	UPROPERTY(VisibleAnywhere, Category = "StatSystem")
	FEnemyStatData EnemyStatData;
	UPROPERTY(VisibleAnywhere, Category = "StatSystem")
	FEnemyDashData EnemyDashData;
	UPROPERTY(VisibleAnywhere, Category = "StatSystem")
	FEnemyAssetData EnemyAssetData;

	UPROPERTY(VisibleAnywhere, Category = "Dash")
	int32 DashCount;

	UPROPERTY(VisibleAnywhere, Category = "Dash")
	int32 CurDashCount = 0;

	bool CanTakeNormal;

	bool CanTakeDash;

	AStageManager* GetStageManager();


	void LookAtPlayer();

	bool bInView = false;

	//Camera_Map 캐스팅함수
	ECamDirection GetCameraDirection();
	//죽은 후 이동함수
	void MoveOut();
	//카메라 이동방향 
	FVector MoveDirection;
	//죽은 후 타이머
	FTimerHandle DeathMoveTimer;
	//플레이어에 의해 죽은 후 시간(이동함수 호출 시간)
	float TimeElapsed = 0.0f;

	void CheckGround();

	void HitReaction(float Damage, EJudgementResult JudgementResult);

	void SpawnDashFx(UNiagaraSystem* Fx, UNiagaraComponent* &NiagaraComponent, const FVector& StartVector, const FVector& EndVector, FVector FxScaleMultiplier = FVector(1.0f, 1.0f, 1.0f), float EffectLocationAlpha = 0.3f);

	UPROPERTY()
	ACamera_Map* UnitList_Manager;
};