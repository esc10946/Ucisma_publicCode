// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "System/CommonTypes.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h" 
#include "EnemyDataTable.generated.h"

/**
 * 
 */
USTRUCT()
struct FEnemyDashData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Dash Count"))
	int32 D_DashCount;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Dash Beat"))
	float D_DashBeat;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Dash Distance"))
	float D_DashDistance;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Has Dash Particle"))
	bool D_HasDashFx;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Dash Fx"))
	UNiagaraSystem* D_DashFx;
};

USTRUCT()
struct FEnemyStatData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 D_MaxHealth;

	UPROPERTY(EditAnywhere)
	int32 D_Damage;
	
	UPROPERTY(EditAnywhere)
	float D_Speed;

	UPROPERTY(EditAnywhere, meta = (ExposeFunctionCategories = "Damage"))
	bool TakeDamage_NormalAttack = true;

	UPROPERTY(EditAnywhere, meta = (ExposeFunctionCategories = "Damage"))
	bool TakeDamage_DashAttack = true;
	
	UPROPERTY(EditAnywhere, meta = (ExposeFunctionCategories = "Damage"))
	EApplyDamageType EnemyApplyDamageType = EApplyDamageType::Always;
	
	UPROPERTY(EditAnywhere, Category = "Dash")
	bool D_HaveDashAbility = true;

	UPROPERTY(EditAnywhere, Category = "Dash", meta = (EditCondition = "D_HaveDashAbility"))
	FEnemyDashData DashData;

};

USTRUCT()
struct FEnemyAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UMaterial* Color;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkelMesh;
};

//USTRUCT()
//struct FMyStruct : public FTableRowBase
//{
//	EEnemyType CurEnemyType;
//
//	FEnemyStatData EnemyStatData;
//
//	FEnemyAssetData EnemyAssetData;
//};

USTRUCT()
struct FEnemyDataStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "EnemyData")
	FName D_ID;

	UPROPERTY(EditAnywhere, Category = "EnemyData")
	ECommonEnemyType D_EnemyType;

	UPROPERTY(EditAnywhere, Category = "EnemyData")
	FEnemyStatData D_EnemyStatData;

	UPROPERTY(EditAnywhere, Category = "EnemyData")
	FEnemyAssetData D_EnemyAssetData;
};