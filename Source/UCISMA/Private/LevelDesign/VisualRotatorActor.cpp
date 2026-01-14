// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesign/VisualRotatorActor.h"

// Sets default values
AVisualRotatorActor::AVisualRotatorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    bHidden = true;
    bIsEditorOnlyActor = true; // 게임에서 안 보이게
    SetActorEnableCollision(false);

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    RootComponent = VisualMesh;

    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetVisibility(false);
    VisualMesh->SetCastShadow(false);

    // 기본 원형 메시 설정 (엔진 메시나 커스텀 메시 경로)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Core/MapDesign/RotatorVisualize/SM_RotatorVisual.SM_RotatorVisual'"));
    if (MeshAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(MeshAsset.Object);
        VisualMesh->SetWorldScale3D(FVector(1.f, 1.f, 1.f)); // 평평하게
    }
}

// Called when the game starts or when spawned
void AVisualRotatorActor::BeginPlay()
{
	Super::BeginPlay();
#if !WITH_EDITOR
    Destroy(); // 게임 시작 시 제거
#endif
}

void AVisualRotatorActor::SetRotationVisual(const FQuat& InRotation)
{
    SetActorRotation(InRotation);
    SetIsTemporarilyHiddenInEditor(false);
    VisualMesh->SetVisibility(true);
}

void AVisualRotatorActor::AttachToTarget(AActor* TargetActor)
{
    if (TargetActor)
    {
        FVector LocationToPlace = TargetActor->GetActorLocation();
        SetActorLocation(LocationToPlace);
        //SetIsTemporarilyHiddenInEditor(false);
    }
}

void AVisualRotatorActor::Hide()
{
    SetIsTemporarilyHiddenInEditor(true);
    VisualMesh->SetVisibility(false);
}

