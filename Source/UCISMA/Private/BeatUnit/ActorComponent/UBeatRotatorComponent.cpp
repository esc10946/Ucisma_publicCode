// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/ActorComponent/UBeatRotatorComponent.h"
#include "BeatUnit/BeatUnitActor.h"
#include "Manager/StageManager.h"
#include "LevelDesign/VisualRotatorActor.h"

// Sets default values for this component's properties
UUBeatRotatorComponent::UUBeatRotatorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	AActor* Owner = GetOwner();
	if (Owner) selfActor = Owner;
}

#if WITH_EDITOR

TWeakObjectPtr<AVisualRotatorActor> UUBeatRotatorComponent::SharedVisualizer = nullptr;

AVisualRotatorActor* UUBeatRotatorComponent::GetOrCreateVisualizer()
{
	if (SharedVisualizer.IsValid())
	{
		return SharedVisualizer.Get();
	}

	UWorld* World = GetWorld();
	if (!World || World->IsGameWorld()) return nullptr;

	// 시각화 액터 생성
	FActorSpawnParameters Params;
	Params.Name = TEXT("SharedVisualRotatorActor");
	Params.ObjectFlags = RF_Transient; // 저장 안됨
	AVisualRotatorActor* NewVisualizer = World->SpawnActor<AVisualRotatorActor>(AVisualRotatorActor::StaticClass(), Params);
	if (NewVisualizer)
	{
		NewVisualizer->SetActorRotation(selfActor->GetActorForwardVector().Rotation());
		SharedVisualizer = NewVisualizer;
	}

	return NewVisualizer;
}

void UUBeatRotatorComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	if (!PropertyChangedEvent.Property) return;

	FName ActivateMemberNodeName = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName();
	//LOG_KHJ(Error, TEXT("PostEditChangeChainProperty MemberNodeName: %s"), *ActivateMemberNodeName.ToString());

	if (GET_MEMBER_NAME_CHECKED(UUBeatRotatorComponent, rotateValue) == ActivateMemberNodeName) {
		FQuat Quat = rotateValue.Quaternion();

		FVector Axis;
		float Angle;
		Quat.ToAxisAndAngle(Axis, Angle);
		VisualRotator(-Axis);
	}
}

void UUBeatRotatorComponent::VisualRotator(FVector Axis)
{
	AVisualRotatorActor* Visualizer = GetOrCreateVisualizer();
	if (!Visualizer || !selfActor) return;

	// 5. Visualizer에 적용
	Visualizer->AttachToTarget(selfActor);
	Visualizer->SetRotationVisual(Axis.Rotation().Quaternion());
}

void UUBeatRotatorComponent::EndVisual()
{
	if (SharedVisualizer.IsValid())
	{
		SharedVisualizer->Hide();
	}
}
#endif

// Called when the game starts
void UUBeatRotatorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	StageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
	StageManager->BeatUpdateListeners.AddDynamic(this, &UUBeatRotatorComponent::BeatUpdate);

	EnTime = 0.f;
	rotateValueQuat = rotateValue.Quaternion();
	rotateStartQuat = selfActor->GetActorRotation().Quaternion();
	/*ABeatUnitActor *Actor = Cast<ABeatUnitActor>(selfActor);
	if (Actor) {

	}*/
}

void UUBeatRotatorComponent::BeatUpdate(int QuaterIndex) {

	if (QuaterIndex) return;
	int beatIndex = StageManager->GetBeatIndex();

	if (rotateStartBeat == beatIndex &&
		rotateStartQuaterBeat == QuaterIndex)
	{
		//StartRotation(durationTime);
		GetWorld()->GetTimerManager().SetTimer(rotateTimerHandle, this, &UUBeatRotatorComponent::StartRotation, 0.01f, true);
	}
}

void UUBeatRotatorComponent::StartRotation()
{
	if (!StageManager || !selfActor) return;

	float SecondsPerBeat = StageManager->GetSecondPerBeat();
	const float TotalDuration = rotateDurationBeat * SecondsPerBeat;

	EnTime += 0.01f;

	float Alpha = EnTime / TotalDuration;
	if (Alpha >= 1.0f)
	{
		Alpha = 1.0f;
		GetWorld()->GetTimerManager().ClearTimer(rotateTimerHandle); // 회전 종료
	}

	FQuat NewRot = FQuat::Slerp(rotateStartQuat, rotateValueQuat, Alpha); // Quaternion 보간
	selfActor->SetActorRotation(NewRot);
}
