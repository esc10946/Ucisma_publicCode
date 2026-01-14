// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/BeatUnitActorMove.h"
#include "Components/WidgetComponent.h"
#include "WarningIndicator.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API AWarningIndicator : public ABeatUnitActorMove
{
	GENERATED_BODY()
public:
    AWarningIndicator();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    virtual void BeatUpdate(int QuaterIndex) override;

    virtual void DeactiveUnit() override;

    UPROPERTY(EditDefaultsOnly, Category = "Design Settings")
    TSubclassOf<UUserWidget> WarningWidgetClass;

private:
    bool bIsOn = true;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UWidgetComponent* WarningWidget;

    UMaterialInstanceDynamic* WarningMaterial;

    void UpdateWarningImage();

    int32 CurrentBeat = 0;  // 반드시 실제 비트 기준 값으로
};
