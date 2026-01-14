// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitCharacter.h"
#include "Core/MyGameInstance.h"
#include "Manager/StageManager.h"

// Sets default values
ABeatUnitCharacter::ABeatUnitCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	QuaterBeatIndex = 0;
	BeatIndex = 0;
	Tags.Add(FName("BeatUnit"));
	bCallBeatUpdateOnSpawn = false;
}

// Called when the game starts or when spawned
void ABeatUnitCharacter::BeginPlay()
{
	Super::BeginPlay();
	StageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
}

// Called every frame
void ABeatUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Begin이 완료될 경우 StageManager에 로딩 완료 전달
}

// Called to bind functionality to input
void ABeatUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABeatUnitCharacter::BeatUpdate(int QuaterIndex)
{
	FString ActorName = GetName();

	//UE_LOG(LogTemp, Warning, TEXT("BeatUnitCharacter > BeatUpdate > %s BeatUpdate Execute"), *ActorName);
}

void ABeatUnitCharacter::DeactiveUnit()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeatUnitCharacter > DeactiveUnit > Execute"));
	bIsLiving = false;

	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
}

void ABeatUnitCharacter::ActiveUnit()
{
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	bIsLiving = true;
	StageManager->BeatUpdateListeners.AddDynamic(this, &ABeatUnitCharacter::BeatUpdate);
	if (bCallBeatUpdateOnSpawn)
	{
		BeatUpdate(StageManager->GetQuaterBeatIndex());
	}
}


void ABeatUnitCharacter::LivingUnit()
{
	bIsLiving = false;
}
