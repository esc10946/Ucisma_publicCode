// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitPawn.h"
#include "Core/MyGameInstance.h"
#include "Manager/StageManager.h"

// Sets default values
ABeatUnitPawn::ABeatUnitPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	QuaterBeatIndex = 0;
	BeatIndex = 0;
	Tags.Add(FName("BeatUnit"));
	bCallBeatUpdateOnSpawn = false;
}

// Called when the game starts or when spawned
void ABeatUnitPawn::BeginPlay()
{
	Super::BeginPlay();
	StageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
}

// Called every frame
void ABeatUnitPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Begin이 완료될 경우 StageManager에 로딩 완료 전달
}

// Called to bind functionality to input
void ABeatUnitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ABeatUnitPawn::BeatUpdate(int QuaterIndex)
{
	FString ActorName = GetName();

	//UE_LOG(LogTemp, Warning, TEXT("BeatUnitPawn > BeatUpdate > %s BeatUpdate Execute"), *ActorName);
}

void ABeatUnitPawn::DeactiveUnit()
{
	//UE_LOG(LogTemp, Warning, TEXT("BeatUnitPawn > DeactiveUnit > Execute"));
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	bIsLiving = false;
}

void ABeatUnitPawn::ActiveUnit()
{
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	bIsLiving = true;
	StageManager->BeatUpdateListeners.AddDynamic(this, &ABeatUnitPawn::BeatUpdate);
	if (bCallBeatUpdateOnSpawn)
	{
		BeatUpdate(StageManager->GetQuaterBeatIndex());
	}
}

