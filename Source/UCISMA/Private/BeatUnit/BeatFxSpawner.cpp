// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatFxSpawner.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"


ABeatFxSpawner::ABeatFxSpawner()
{
	FxComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FxComponent"));
	FxComponent->SetupAttachment(RootComponent);
}
//해당 유닛을 활성화 하는 함수
void ABeatFxSpawner::ActiveUnit()
{
	Super::ActiveUnit();
	if (!(FxComponent && SpawnFx))	return;
	
	FxComponent->SetAsset(SpawnFx); 
	FxComponent->SetActive(true);
}

//해당 유닛을 비활성화 하는 함수
void ABeatFxSpawner::DeactiveUnit()
{
	Super::DeactiveUnit();
	if (!(FxComponent && SpawnFx))	return;
	FxComponent->SetActive(false);
}
