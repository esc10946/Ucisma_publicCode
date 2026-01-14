// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "BeatUnitActor.h"
#include "BeatUnitActor_FadeOut.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API ABeatUnitActor_FadeOut : public ABeatUnitActor
{
	GENERATED_BODY()
public:
    ABeatUnitActor_FadeOut();
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void BeatUpdate(int QuaterIndex) override;

    int BeatCount;
    float BeatDuration;
    float TimeLine;
    bool bIsInitialized;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int FadeTiming;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UUserWidget* FadeWidget;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ChangeScene();

private:
    float TargetOpacity;

    void UpdateWidgetOpacity(float alpha);
};
