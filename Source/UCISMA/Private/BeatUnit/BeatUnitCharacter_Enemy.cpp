// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitCharacter_Enemy.h"

void ABeatUnitCharacter_Enemy::TakeDamagePlayer(float Damage, EJudgementResult Judgement, AActor* DamageCauser)
{
	LOG_KHJ(Error, TEXT("[Attack Log] Damage: %.1f | Judgement: %d | Causer: %s"),
		Damage,
		static_cast<int32>(Judgement),
		*DamageCauser->GetName()
	);
}
