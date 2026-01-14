// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "System/CommonTypes.h"
#include "ProjectileInterface.h"
#include "LinearProjectile.generated.h"

UCLASS()
class UCISMA_API ALinearProjectile : public AActor, public IProjectileInterface
{
	GENERATED_BODY()

public:
	ALinearProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Shoot(FProjectileInformation ProjectileInfo) override;
	virtual void ProjectileHitEvent();

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void DestroyProjectile() override;

private:
	//������ �ʱ�ȭ
	void ConstructData();
	void ConstructComponent();
	virtual void ControlProjectile(const FProjectileInformation& ProjectileInfo, float DeltaTime);
	virtual void MoveProjectile(const FProjectileInformation& ProjectileInfo, float DeltaTime);
	bool IsOverRange(const FProjectileInformation& ProjectileInfo);
	void Init();

	//����ü�� �����ӿ� ���� ����
	FProjectileInformation ProjectileInformation;
	//Init ����
	bool bWasShoot;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* HitCollisionComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ProjectileMeshComponent;

	UPROPERTY(EditAnywhere)
	int ProjectileDamage;


};
