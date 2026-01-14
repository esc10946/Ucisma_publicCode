// Fill out your copyright notice in the Description page of Project Settings.


#include "BT/BTService_Dash.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "AIController.h"

void UBTService_Dash::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyBase* Enemy = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());

	/*UE_LOG(LogTemp, Warning, TEXT("D_HaveDashAbility :%d"), Enemy->EnemyStatData.D_HaveDashAbility);
	UE_LOG(LogTemp, Warning, TEXT("Candash :%d"), Enemy->Candash);*/

	if (Enemy) {
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanDash"), Enemy->CanDash);
	}
	else {
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanDash"), false);
	}
}
