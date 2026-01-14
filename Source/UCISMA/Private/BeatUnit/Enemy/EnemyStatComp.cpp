
// Fill out your copyright notice in the Description page of Project Settings.
#include "BeatUnit/Enemy/EnemyStatComp.h"

// Sets default values for this component's properties
UEnemyStatComp::UEnemyStatComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	// ...
}


// Called when the game starts
void UEnemyStatComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UEnemyStatComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UEnemyStatComp::DecreaseHp(float value)
{
	CurrentHealth -= value;

	if (CurrentHealth <= 0) {
		CurrentHealth = 0;
		return true;
	}
	else {
		return false;
	}
}

void UEnemyStatComp::IncreaseHp(float value)
{
	CurrentHealth += value;

	if (CurrentHealth > MaxHealth) {
		CurrentHealth = MaxHealth;
	}
}

float UEnemyStatComp::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}
