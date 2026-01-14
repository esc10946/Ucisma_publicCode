// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "System/CommonTypes.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "ShootingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API AShootingEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	virtual void ActiveUnit() override;
	virtual void BeatUpdate(int QuaterIndex) override;
	void ShootProjectile(const FProjectileInformation& ProjectileInformation);

protected:
	class IProjectileInterface* GetProjectile();
private:
	AShootingEnemy();
	virtual void HandleShootingPattern(int QuaterBeat);
	virtual void ShootNormalPattern(int QuaterBeat);

	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	TSubclassOf<class ALinearProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ShootingCountPerBeat;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ShootingCycleRepeatCount;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ShootingCycleCoolTimeBeat;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ProjectileCountPerShoot;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ProjectileSpeed;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	int ProjectileRange;
	UPROPERTY(EditAnywhere, Category = "Design Settings|Projectile")
	float ProjectileAngleGap;

	int CurrentShootingCycleRepeatCount;
	int CurrentShootingCycleCoolTimeBeat;

	bool bIsCoolTime = false;

	
};
