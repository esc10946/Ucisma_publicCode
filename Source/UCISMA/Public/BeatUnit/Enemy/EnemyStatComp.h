// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Components/ActorComponent.h"
#include "EnemyStatComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UCISMA_API UEnemyStatComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyStatComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//void SetNewLevel(int32 Level);

	UPROPERTY(EditAnywhere, Category = "Stat")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHealth;

	////////////////ÇÔ¼ö

	bool DecreaseHp(float value);
	void IncreaseHp(float value);

	float GetHealthPercent() const; // Ã¼·Â ¹éºÐÀ² ¹ÝÈ¯
};