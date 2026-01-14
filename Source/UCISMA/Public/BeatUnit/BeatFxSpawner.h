// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/BeatUnitActor.h"
#include "BeatFxSpawner.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API ABeatFxSpawner : public ABeatUnitActor
{
	GENERATED_BODY()
public:
	ABeatFxSpawner();
	//해당 유닛을 비활성화 하는 함수
	virtual void DeactiveUnit() override;
	//해당 유닛을 활성화 하는 함수
	virtual void ActiveUnit() override;


	UPROPERTY(EditAnywhere, Category = "Design Settings|Fx")
	class UNiagaraSystem* SpawnFx;
private:
	class UNiagaraComponent* FxComponent;
	
};
