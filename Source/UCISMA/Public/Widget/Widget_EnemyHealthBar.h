// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "Widget/ActorWidgetInterface.h"
#include "Widget_EnemyHealthBar.generated.h"



/**
 * 
 */
UCLASS()
class UCISMA_API UWidget_EnemyHealthBar : public UUserWidget, public IActorWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void InitWidget(AActor* Parent) override;
	virtual void UpdateValue() override;
	UFUNCTION(BlueprintCallable)
	virtual void RemoveWidget() override;
	UFUNCTION(BlueprintCallable)
	void CreateDividers(float DividerInterval, float Value, float DividerThicknessMax, float DividerThicknessMin);
	void FadeHp(float OriginValue, float TargetValue);

	UPROPERTY(BlueprintReadWrite)
	float FadeTime;
	UPROPERTY(BlueprintReadWrite)
	float FadeHpLerpMax;
	UPROPERTY(BlueprintReadWrite)
	float FadeHpLerpMin;
	UPROPERTY(BlueprintReadWrite)
	class AStageManager* StageManager;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UProgressBar* HpBar_Fade;


private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void TakeEnemyData();
	

	UPROPERTY(VisibleAnywhere)
	class AEnemyBase* ParentEnemy;
	

	float MaxHp;
	float PreHp;
	float CurrentHp;



	bool bIsRemove;

	UPROPERTY(EditAnywhere)
	float HpDividerInterval;
	float HpDividerThicknessMin;
	float HpDividerThicknessMax;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* DividerHorizontalBox;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimPadeOut;
};
