// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/Enemy/Enemy_AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemy_AIController::AEnemy_AIController()
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree> aiBehavior(TEXT("BehaviorTree'/Game/Enemy/AI/BT_AI.BT_AI'"));

	if (aiBehavior.Succeeded())
	{
		UE_LOG(LogTemp, Display, TEXT("aiBehavior.Succeeded()"));
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Succeeded()"));
		EnemyBehaviorTree = aiBehavior.Object;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("aiBehavior.silpa"));
	}
}

void AEnemy_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyBehaviorTree != nullptr) 
	{
		RunBehaviorTree(EnemyBehaviorTree);

		APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
		if (playerPawn) {

			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), playerPawn->GetActorLocation());

			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetObject"), playerPawn);

		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Fail!!!!!!!!!!!!!!!!!!!!!"));
		}

	}
}