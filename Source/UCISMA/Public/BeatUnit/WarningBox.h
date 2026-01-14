// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatUnit/BeatUnitActorMove.h"
#include "WarningBox.generated.h"

/**
 * 
 */
UCLASS()
class UCISMA_API AWarningBox : public ABeatUnitActorMove
{
    GENERATED_BODY()

public:
    AWarningBox();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Design Settings")
    UMaterialInterface* BoxMaterial;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* BoxMesh;
};
