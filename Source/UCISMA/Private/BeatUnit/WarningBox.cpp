#include "BeatUnit/WarningBox.h"

AWarningBox::AWarningBox()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 직육면체 메시 생성 (기본 큐브 사용)
    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
    RootComponent = BoxMesh;

    // 🔹 충돌 비활성화 (모든 오브젝트와 상호작용 X)
    BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxMesh->SetCastShadow(false);
    // 🔹 기본 메시 할당 (언리얼 기본 큐브)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        BoxMesh->SetStaticMesh(CubeMesh.Object);
    }
}

// ===== 게임 시작 =====
void AWarningBox::BeginPlay()
{
    Super::BeginPlay();

    if (BoxMaterial)
    {
        BoxMesh->SetMaterial(0, BoxMaterial);
    }

    // 초기에는 안 보이게
    SetActorHiddenInGame(true);
}
