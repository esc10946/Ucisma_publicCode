// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/WarningBox.h"
#include "BeatUnitActor_VFXBox.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API ABeatUnitActor_VFXBox : public AWarningBox
{
	GENERATED_BODY()
private:
	virtual void ActiveUnit() override;

	UPROPERTY(EditAnywhere, Category = "Design Settings")
	FColor color;

	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY()
	FTimerHandle OpacityTimerHandle;

	float OpacityDuration = 0.f;
	float ElapsedTime = 0.f;

	UFUNCTION()
	void ChangeOpacity();
	UFUNCTION()
	void UpdateOpacity();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
