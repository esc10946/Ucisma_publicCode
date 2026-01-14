// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/Enemy/ShootingEnemy.h"
#include "Projectile/LinearProjectile.h"
#include "PlayerCharacter.h"
#include "Projectile/ProjectileInterface.h"

AShootingEnemy::AShootingEnemy()
{
	bCallBeatUpdateOnSpawn = true;
	bIsCoolTime = false;
}

void AShootingEnemy::ActiveUnit()
{
	Super::ActiveUnit();

	if (ShootingCycleRepeatCount == 0)
	{
		ShootingCycleRepeatCount = std::numeric_limits<int>::max();
	}
	CurrentShootingCycleRepeatCount = ShootingCycleRepeatCount;
	CurrentShootingCycleCoolTimeBeat = 0;
}

void AShootingEnemy::BeatUpdate(int QuaterIndex)
{
	HandleShootingPattern(QuaterIndex);
}

void AShootingEnemy::HandleShootingPattern(int QuaterBeat)
{
	if (QuaterBeat == 0)
	{
		if (CurrentShootingCycleRepeatCount == 0 && !bIsCoolTime)
		{
			CurrentShootingCycleCoolTimeBeat = ShootingCycleCoolTimeBeat;
			bIsCoolTime = true;
		}
		if (CurrentShootingCycleCoolTimeBeat == 0 && bIsCoolTime)
		{
			CurrentShootingCycleRepeatCount = ShootingCycleRepeatCount;
			bIsCoolTime = false;
		}
		if (bIsCoolTime)
		{
			CurrentShootingCycleCoolTimeBeat--;
		}
		else
		{
			CurrentShootingCycleRepeatCount--;
		}
		
	}

	if (bIsCoolTime)
	{
		return;
	}

	ShootNormalPattern(QuaterBeat);
}

void AShootingEnemy::ShootNormalPattern(int QuaterBeat)
{
	if (QuaterBeat == 0 && ShootingCountPerBeat <= 0)
	{
		return;
	}
	if (QuaterBeat == 1 && ShootingCountPerBeat != 4)
	{
		return;
	}
	if (QuaterBeat == 2 && !(ShootingCountPerBeat == 2 || ShootingCountPerBeat == 4))
	{
		return;
	}
	if (QuaterBeat == 3 && ShootingCountPerBeat != 4)
	{
		return;
	}

	FRotator FirstShootingAngle = this->GetActorRotation() + FRotator(0, ProjectileAngleGap / 2 * (ProjectileCountPerShoot - 1) * -1, 0);
	FProjectileInformation TempProjectileInformation;
	TempProjectileInformation.ProjectileAngle = this->GetActorRotation();
	TempProjectileInformation.ProjectileSpeed = ProjectileSpeed;
	TempProjectileInformation.ProjectileRange = ProjectileRange;
	TempProjectileInformation.StartPosition = this->GetActorLocation() + FVector::ZAxisVector * -30;
	TempProjectileInformation.TargetClass = APlayerCharacter::StaticClass();

	for (int ProjectileNum = 0; ProjectileNum < ProjectileCountPerShoot; ProjectileNum++)
	{
		TempProjectileInformation.ProjectileAngle = FirstShootingAngle + FRotator(0, ProjectileAngleGap * ProjectileNum, 0);
		ShootProjectile(TempProjectileInformation);
	}
}

void AShootingEnemy::ShootProjectile(const FProjectileInformation& ProjectileInformation)
{
	IProjectileInterface* Projectile = GetProjectile();
	if (!Projectile)
	{
		LOG_DEBUG(Display, TEXT("Shoot Enemy can't get projectile (Maybe destroyed as soon as created)"));
		return;
	}

	Projectile->Shoot(ProjectileInformation);
}

IProjectileInterface* AShootingEnemy::GetProjectile()
{
	IProjectileInterface* Projectile = GetWorld()->SpawnActor<ALinearProjectile>(ProjectileClass, GetActorLocation(), GetActorRotation());

	return Projectile;
}

