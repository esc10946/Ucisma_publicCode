// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/LinearProjectile.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
#include "Components/MeshComponent.h"

// Sets default values
ALinearProjectile::ALinearProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructData();
	ConstructComponent();
}

void ALinearProjectile::ConstructData()
{
	bWasShoot = false;
}

void ALinearProjectile::ConstructComponent()
{
	HitCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = HitCollisionComponent;

	// ũ�� ���� (���ϴ� ������ ����)
	HitCollisionComponent->InitCapsuleSize(34.f, 34.f);
	HitCollisionComponent->SetCollisionProfileName(TEXT("EnemyProjectile"));

	// �޽� ������Ʈ ���� �� ĸ���� ����
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	ProjectileMeshComponent->SetupAttachment(HitCollisionComponent);

	// �޽� �浹 ��Ȱ��ȭ (ĸ���� �浹�� �ñ����)
	ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

// Called every frame
void ALinearProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bWasShoot)
	{
		return;
	}
	//����ü�� ����
	ControlProjectile(ProjectileInformation, DeltaTime);
}

void ALinearProjectile::Init()
{
	HitCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ALinearProjectile::OnOverlapBegin);
}

void ALinearProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ProjectileInformation.TargetClass) && OtherComp->ComponentHasTag(TEXT("HitRange")))
	{
		if (ProjectileInformation.TargetClass == APlayerCharacter::StaticClass())
		{
			APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(OtherActor);
			HitPlayer->Take_HP(-1, this);
		}
		ProjectileHitEvent();
	}
}

void ALinearProjectile::ControlProjectile(const FProjectileInformation& ProjectileInfo, float DeltaTime)
{
	MoveProjectile(ProjectileInfo, DeltaTime);

	//��Ÿ� �ʰ� Ȯ�� �� ����ü �ı� ����
	if (IsOverRange(ProjectileInfo))
	{
		DestroyProjectile();
	}
}

void ALinearProjectile::MoveProjectile(const FProjectileInformation& ProjectileInfo, float DeltaTime)
{
	FVector Direction = ProjectileInfo.ProjectileAngle.Vector();
	FVector DeltaDistance = Direction * ProjectileInfo.ProjectileSpeed * DeltaTime;

	this->SetActorLocation(GetActorLocation() + DeltaDistance);
}

void ALinearProjectile::DestroyProjectile()
{
	this->Destroy();
}

bool ALinearProjectile::IsOverRange(const FProjectileInformation& ProjectileInfo)
{
	FVector StartLocation = ProjectileInfo.StartPosition;	
	FVector ProjectileLocation = this->GetActorLocation();
	float Distance = FVector::Dist(StartLocation, ProjectileLocation);

	if (Distance >= ProjectileInfo.ProjectileRange)
	{
		return true;
	}
	return false;
}

void ALinearProjectile::ProjectileHitEvent()
{
	//LOG_PSJ(Display, TEXT("Hit Projectile!"));
	DestroyProjectile();
}

void ALinearProjectile::Shoot(FProjectileInformation ProjectileInfo)
{
	ProjectileInformation = ProjectileInfo;
	this->SetActorLocation(ProjectileInformation.StartPosition);
	this->SetActorRotation(ProjectileInformation.ProjectileAngle);
	bWasShoot = true;
}