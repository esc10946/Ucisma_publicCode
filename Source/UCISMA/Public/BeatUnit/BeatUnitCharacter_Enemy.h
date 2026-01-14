// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "CoreMinimal.h"
#include "System/CommonTypes.h"
#include "BeatUnit/BeatUnitCharacter.h"
#include "BeatUnitCharacter_Enemy.generated.h"

/**
 * 
 */
UENUM()
enum class EEnemyType : uint8
{
	None UMETA(DisplayName = "None"),
	Common UMETA(DisplayName = "Common"),
	Boss UMETA(DisplayName = "Boss")
};

UCLASS()
class UCISMA_API ABeatUnitCharacter_Enemy : public ABeatUnitCharacter
{
	GENERATED_BODY()
public:
	virtual void TakeDamagePlayer(float Damage, EJudgementResult Judgement, AActor* DamageCauser);

	const bool bIsBossMonster() { return CurEnemyType == EEnemyType::Boss; };
	const bool bIsCommonMonster() { return CurEnemyType == EEnemyType::Common; };

protected:
	void SetEnemyType(EEnemyType EnemyType) { this->CurEnemyType = EnemyType; };

private:
	UPROPERTY(VisibleAnywhere, Category = "Design Settings|EnemyType")
	EEnemyType CurEnemyType;
};
