// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor_VFXBox.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/StageManager.h"

void ABeatUnitActor_VFXBox::ActiveUnit()
{
	Super::ActiveUnit();

	if (!BoxMesh) return;

	DynMaterial = UMaterialInstanceDynamic::Create(BoxMesh->GetMaterial(0), this);
	BoxMesh->SetMaterial(0, DynMaterial);

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), color);

	if (UseEndBeatIndex)
	{
		ChangeOpacity();
	}
}

void ABeatUnitActor_VFXBox::ChangeOpacity()
{
	AStageManager* stageManager = Cast<AStageManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AStageManager::StaticClass()));
	if (!stageManager) return;

	const int TotalQuterBeatIndex = (EndQuterBeatIndex + EndBeatIndex * 4) - (QuaterBeatIndex + BeatIndex * 4);
	OpacityDuration = stageManager->GetSecondPerBeat() * TotalQuterBeatIndex / 4;
	ElapsedTime = 0.f;

	GetWorldTimerManager().SetTimer(OpacityTimerHandle, this, &ABeatUnitActor_VFXBox::UpdateOpacity, 0.01f, true);
}

void ABeatUnitActor_VFXBox::UpdateOpacity()
{
	if (!DynMaterial) return;

	ElapsedTime += 0.01f;
	float Alpha = FMath::Clamp(ElapsedTime / OpacityDuration, 0.f, 1.f);
	float NewOpacity = FMath::Lerp(1.0f, 0.0f, Alpha);

	DynMaterial->SetScalarParameterValue("Opacity", NewOpacity);

	if (Alpha >= 1.f)
	{
		GetWorldTimerManager().ClearTimer(OpacityTimerHandle);
	}
}

#if WITH_EDITOR
void ABeatUnitActor_VFXBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ABeatUnitActor_VFXBox, color))
	{
		if (BoxMesh)
		{
			if (!DynMaterial)
			{
				DynMaterial = UMaterialInstanceDynamic::Create(BoxMesh->GetMaterial(0), this);
				BoxMesh->SetMaterial(0, DynMaterial);
			}

			if (DynMaterial)
			{
				DynMaterial->SetVectorParameterValue("BaseColor", color);
			}
		}
	}
}
#endif
