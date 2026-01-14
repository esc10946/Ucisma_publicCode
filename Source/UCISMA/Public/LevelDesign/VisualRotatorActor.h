// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualRotatorActor.generated.h"

UCLASS()
class UCISMA_API AVisualRotatorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualRotatorActor();  

	/** 회전 방향을 시각화하는 메서드 */
	void SetRotationVisual(const FQuat& InRotation);

	/** 시각화 대상 액터에 붙임 */
	void AttachToTarget(AActor* TargetActor);

	/** 시각화 비활성화 */
	void Hide();

protected:
	virtual void BeginPlay() override;

private:
	/** 원형 회전 메시 */
	UPROPERTY()
	UStaticMeshComponent* VisualMesh;

	bool bHidden;
};
