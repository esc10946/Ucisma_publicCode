// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_EnemyHealthBar.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "BeatUnit/Enemy/EnemyDataTable.h"
#include "BeatUnit/Enemy/EnemyStatComp.h"
#include "Manager/StageManager.h"
#include "LevelDesign/GameDataAsset.h"

void UWidget_EnemyHealthBar::InitWidget(AActor* Parent)
{
	ParentEnemy = Cast<AEnemyBase>(Parent);
	StageManager = ParentEnemy->StageManager;
	if (!ParentEnemy)
	{
		LOG_DEBUG(Warning, TEXT("Cast to AEnemyBase Failed"));
	}
	TakeEnemyData();

	UGameDataAsset* GameDataAsset = ParentEnemy->StageManager->GetGameDataAsset();
	
	HpDividerInterval = GameDataAsset->GetDividerInterval();
	HpDividerThicknessMax = GameDataAsset->GetDividerThicknessMax();
	HpDividerThicknessMin = GameDataAsset->GetDividerThicknessMin();

	HpBar->SetPercent(1.0f);
	HpBar_Fade->SetPercent(1.0f);

	CreateDividers(HpDividerInterval, MaxHp, HpDividerThicknessMax, HpDividerThicknessMin);

	bIsRemove = false;
}

void UWidget_EnemyHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidget_EnemyHealthBar::UpdateValue()
{
	PreHp = CurrentHp;
	CurrentHp = ParentEnemy->GetEnemyStat()->CurrentHealth;
	float HpPercent = FMath::Clamp(CurrentHp / MaxHp, 0.0f, 1.0f);
	HpBar->SetPercent(HpPercent);
	if (CurrentHp + 0.1f < PreHp)
	{
		FadeHp(PreHp, CurrentHp);
	}
	if (CurrentHp < 0.1f && !bIsRemove)
	{
		bIsRemove = true;
		RemoveWidget();
	}
}

void UWidget_EnemyHealthBar::FadeHp(float OriginValue, float TargetValue)
{
	FadeTime = 1.0f;
	FadeHpLerpMax = OriginValue/MaxHp;
	FadeHpLerpMin = TargetValue / MaxHp;
}

void UWidget_EnemyHealthBar::RemoveWidget()
{
	PlayAnimation(AnimPadeOut, 0.0f, 1, EUMGSequencePlayMode::Forward, StageManager->GetBeatPerSecond());
}

void UWidget_EnemyHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (FadeTime > -0.1f)
	{
		FadeTime -= InDeltaTime * StageManager->GetBeatPerSecond();
		float FadePercent = FMath::Lerp(FadeHpLerpMin, FadeHpLerpMax, FadeTime);
		HpBar_Fade->SetPercent(FadePercent);
	}
}

void UWidget_EnemyHealthBar::TakeEnemyData()
{
	if (!ParentEnemy)
	{
		LOG_DEBUG(Error, TEXT("Enemy Health Bar can't find Parent Enemy"));

		return;
	}

	FEnemyDataStruct* EnemyData = ParentEnemy->GetEnemyData();
	MaxHp = EnemyData->D_EnemyStatData.D_MaxHealth;

	HpDividerInterval = 25.f;
}

void UWidget_EnemyHealthBar::CreateDividers(float DividerInterval, float Value, float DividerThicknessMax, float DividerThicknessMin)
{
	UBorder* Spacer = NewObject<UBorder>(this);
	FLinearColor TransparentColor(1.0f, 1.0f, 1.0f, 0.0f);
	Spacer->SetBrushColor(TransparentColor);

	UBorder* Divider = NewObject<UBorder>(this);
	FLinearColor BlackColor(0.0f, 0.0f, 0.0f, 1.0f);
	Divider->SetBrushColor(BlackColor);
	float Thickness = FMath::Max(DividerThicknessMin, DividerThicknessMax - (DividerThicknessMax - DividerThicknessMin) / 10 * static_cast<int>(Value / DividerInterval));
	Divider->SetPadding({ Thickness, 0, Thickness, 0 });

	UHorizontalBoxSlot* HorizontalBoxSlot = DividerHorizontalBox->AddChildToHorizontalBox(Spacer);
	HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
	HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
	
	for (int i = 0; i < static_cast<int>(Value / DividerInterval) - 1; i++)
	{
		Divider = NewObject<UBorder>(this);
		Divider->SetBrushColor(BlackColor);
		Divider->SetPadding({ Thickness, 0, Thickness, 0 });

		HorizontalBoxSlot = DividerHorizontalBox->AddChildToHorizontalBox(Divider);
		HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
		HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);

		Spacer = NewObject<UBorder>(this);
		Spacer->SetBrushColor(TransparentColor);
		HorizontalBoxSlot = DividerHorizontalBox->AddChildToHorizontalBox(Spacer);
		HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
		HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
	}


}
