// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCISMA/UCISMA.h"
#include "Components/ActorComponent.h"
#include "UBeatRotatorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCISMA_API UUBeatRotatorComponent : public UActorComponent
{
	GENERATED_BODY()

	class AActor *selfActor;
	static TWeakObjectPtr<class AVisualRotatorActor> SharedVisualizer;

	class AVisualRotatorActor* GetOrCreateVisualizer();

	FQuat rotateValueQuat;
	FQuat rotateStartQuat;
	float EnTime;
	class AStageManager* StageManager;
	FTimerHandle rotateTimerHandle;

	void StartRotation();
public:	
	// Sets default values for this component's properties
	UUBeatRotatorComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	void UpdateVisualization();

	UFUNCTION()
	void BeatUpdate(int QuaterIndex);
public:	
	UFUNCTION(CallInEditor)

	void EndVisual();

	UFUNCTION(CallInEditor)
	void VisualRotator(FVector Axis);

	UPROPERTY(EditAnywhere)
	int rotateStartBeat;
	UPROPERTY(EditAnywhere)
	int rotateStartQuaterBeat;

	UPROPERTY(EditAnywhere)
	float rotateDurationBeat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator rotateValue;
};
