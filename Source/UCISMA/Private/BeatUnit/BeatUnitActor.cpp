// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor.h"
#include "Core/MyGameInstance.h"
#include "Manager/StageManager.h"

// Sets default values
ABeatUnitActor::ABeatUnitActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	QuaterBeatIndex = 0;
	BeatIndex = 0;
	Tags.Add(FName("BeatUnit"));
	bCallBeatUpdateOnSpawn = false;
}

// Called when the game starts or when spawned
void ABeatUnitActor::BeginPlay()
{
	Super::BeginPlay(); 
	StageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
}

// Called every frame
void ABeatUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Begin이 완료될 경우 StageManager에 로딩 완료 전달
}

void ABeatUnitActor::BeatUpdate(int QuaterIndex)
{
	FString ActorName = GetName();

	if (!QuaterIndex) CurrentBeatIndex++;

	if (!UseEndBeatIndex) return;
	if (CurrentBeatIndex >= EndBeatIndex && EndQuterBeatIndex == QuaterIndex ) {
		DeactiveUnit();
	}
}

void ABeatUnitActor::DeactiveUnit()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeatUnitActor > DeactiveUnit > Execute"));
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	bIsLiving = false;
}

void ABeatUnitActor::ActiveUnit()
{
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	bIsLiving = true;
	CurrentBeatIndex = GetBeatIndex();
	StageManager->BeatUpdateListeners.AddDynamic(this, &ABeatUnitActor::BeatUpdate);
	if (bCallBeatUpdateOnSpawn)
	{
		BeatUpdate(StageManager->GetQuaterBeatIndex());
	}
}
