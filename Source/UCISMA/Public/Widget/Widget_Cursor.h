#pragma once

#include "UCISMA/UCISMA.h"
#include "Blueprint/UserWidget.h"
#include "../PlayerCharacter.h"
#include "Widget_Cursor.generated.h"

/**
 *
 */
class APlayerCharacter;

UCLASS()
class UCISMA_API UWidget_Cursor : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 초기화 함수 (UI에 바인딩 및 값 업데이트)
	virtual void NativeConstruct() override;
	//void Update_HP(APlayerCharacter* PlayerCharacter);
public:
	// 캔버스 패널 선언
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;

	// 이미지 위젯 선언
	UPROPERTY(meta = (BindWidget))
	class UImage* HPCell_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* HPCell_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* HPCell_3;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor decrease_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor increase_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor delete_color;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor default_color;

	void Event_Increase(int HP);

	UFUNCTION(BlueprintCallable)
	void Event_Decrease(int HP);

	void Init();
	void Increase_change_color(UImage* TargetImage);
	void Decrease_change_color(UImage* TargetImage);
	void ResetColor(UImage* TargetImage);
	void DefaultColor(UImage* TargetImage);


private:
	UPROPERTY()
	APlayerCharacter* PlayerCharacterInstance;

	UFUNCTION()
	void HandleStatsChanged(int Player_Current_HP, int changer);
	void FindPlayerCharacter();

	//hp 중첩 버그 해소
	void HP_Refresh(int Player_Current_HP);


};