// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "BeatUnitActor.h"
#include "Obstacles.generated.h"

UCLASS()
class UCISMA_API AObstacles : public ABeatUnitActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstacles();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Power")
	float KnockbackPower;

	UPROPERTY(EditAnywhere, Category = "Power")
	int32 Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool IsLiving;

	UPROPERTY()
	ACharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void KnockBack();
	
	UFUNCTION(BlueprintCallable)
	void DamageApply();
};
