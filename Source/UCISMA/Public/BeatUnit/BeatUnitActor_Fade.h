// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/BeatUnitActor.h"
#include "BeatUnitActor_Fade.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API ABeatUnitActor_Fade : public ABeatUnitActor
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
	virtual void ActiveUnit() override;
	virtual void DeactiveUnit() override;
public:
	ABeatUnitActor_Fade();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "time")
	float FadeIn_Beat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "time")
	float FadeOut_Beat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "color")
	bool bisblack;



	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Event")
	void SpawnWidget();
private:


};
