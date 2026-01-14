// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h" 
#include "../PlayerCharacter.h"
#include "Widget_NewCursor.generated.h"

/**
 * 
 */
class APlayerCharacter;


UCLASS()
class UCISMA_API UWidget_NewCursor : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;


public:
	void Event_Increase(int HP);

	UFUNCTION(BlueprintCallable)
	void Event_Decrease(int HP);

	void InitColor();
	void InitPlayerData();
	void InitData();
	//void Increase_change_color(UProgressBar* TargetImage);
	//void Decrease_change_color(UProgressBar* TargetImage);
	//void ResetColor(UProgressBar* TargetImage);
	//void DefaultColor(UProgressBar* TargetImage);


	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Dash_Bar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Health_Bar_1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Health_Bar_2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* Health_Bar_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor decrease_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor increase_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor delete_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor default_color;


private:
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UFUNCTION()
	void HandleStatsChanged(int Player_Current_HP, int changer);


	void HP_Refresh(int Player_Current_HP);

	void Update_Health(APlayerCharacter* PlayerCharacter);
	void Update_Dash(APlayerCharacter* PlayerCharacter);

	float beatsecond = 1.0f;

	//void Fade_HPCell(UProgressBar* HPCell, float Duration, bool binout);

	void FadeHeal(float TickDeltaTime);

	FTimerHandle FadeTimerHandle;

	FLinearColor DashStartColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);     // �ϴû�
	FLinearColor DashTargetColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���
	float ElapsedTime_Dash = 0.0f; //����ð�
	bool bIsDashTrans = false;
	float LastStamina = 0.0f;

	int HealingCellIndex;
	float HealFadeAlpha;

	bool bisfading = false;
	bool bIsHealFading;

	bool bisattacked = false;
};
