// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "CommonTypes.generated.h"

//튜토리얼 단계 나눠놓음
UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
	Step0 = 0 UMETA(DisplayName = "0~7 BeatIndex"),
	Step1 = 1 UMETA(DisplayName = "8~15 BeatIndex"),
	Step2 = 2 UMETA(DisplayName = "16~23 BeatIndex"),
	Step3 = 3 UMETA(DisplayName = "24~31 BeatIndex"),
	Step4 = 4 UMETA(DisplayName = "32~39 BeatIndex"),
	Step5 = 5 UMETA(DisplayName = "40~47  BeatIndex"),
	Step6 = 6 UMETA(DisplayName = "48~55  BeatIndex"),
	Step7 = 7 UMETA(DisplayName = "56~63  BeatIndex"),
	Step8 = 8 UMETA(DisplayName = "64~71  BeatIndex"),
	Step9 = 9 UMETA(DisplayName = "72~79 BeatIndex"),
	Step10 = 10 UMETA(DisplayName = "80~87 BeatIndex"),
	StepFinal = 11 UMETA(DisplayName = "88~96 BeatIndex")
};

UENUM(BlueprintType)
enum class EUIType : uint8
{
	SongProcessBar = 0 UMETA(DisplayName = "SongProcessBar"),
	HealthBar = 1 UMETA(DisplayName = "HealthBar"),
	BeatBar = 2 UMETA(DisplayName = "BeatBar"),
	BeatNode = 3 UMETA(DisplayName = "BeatBarNode"),
	Score = 4 UMETA(DisplayName = "Score"),
	Combo = 5 UMETA(DisplayName = "Combo"),
	State = 6 UMETA(DisplayName = "State")
};

//효과음 종류
UENUM(BlueprintType)
enum class ESfx : uint8
{
	AttackHitNormal UMETA(DisplayName = "Attack Hit Normal"),
	AttackHitGood UMETA(DisplayName = "Attack Hit Good"),
	AttackHitGreat UMETA(DisplayName = "Attack Hit Great"),
	AttackHitPerfect UMETA(DisplayName = "Attack Hit Perfect"),
	DashNormal UMETA(DisplayName = "Dash Nomral"),
	DashPerfect UMETA(DisplayName = "Dash Perfect"),
	PlayerHealed UMETA(DisplayName = "Player Healed"),
	PlayerDamaged UMETA(DisplayName = "Player Damaged"),
	AttackInRange UMETA(DisplayName = "Attack In Range"),
	AttackOutRange UMETA(DisplayName = "Attack Out Range"),
	PurificationFailNormal UMETA(DisplayName = "Purification Fail Normal"),
	PurificationFailDanger UMETA(DisplayName = "Purification Fail Danger"),
	DashFailed UMETA(DisplayName = "Dash Failed"),
	BossDied UMETA(DisplayName = "Boss Died"),
	MetronomeSound UMETA (DisplayName = "Metronome Sound")
};

//스테이지 타입
UENUM(BlueprintType)
enum class EStageIndex : uint8
{
	AdjustInputOffset = 200 UMETA(DisplayName = "AdjustInputOffset"),
	StageTutorial = 0 UMETA(DisplayName = "StageTutorial"),
	Stage_1_1 UMETA(DisplayName = "1-1"),
	Stage_1_2 UMETA(DisplayName = "1-2"),
	Stage_1_3 UMETA(DisplayName = "1-3"),
	Stage_2_1 UMETA(DisplayName = "2-1"),
	Stage_2_2 UMETA(DisplayName = "2-2"),
	Stage_2_3 UMETA(DisplayName = "2-3"),
	Stage_3_1 UMETA(DisplayName = "3-1"),
	Stage_3_2 UMETA(DisplayName = "3-2"),
	Stage_3_3 UMETA(DisplayName = "3-3"),
	Stage_4_1 UMETA(DisplayName = "4-1"),
	Stage_4_2 UMETA(DisplayName = "4-2"),
	Stage_4_3 UMETA(DisplayName = "4-3")
};

//판정 타입
UENUM()
enum EJudgementResult : uint16
{
	//대미지가 높은 순
	AttackPerfect = 4, 
	AttackGreat = 3, 
	AttackGood = 2, 
	AttackNormal = 1, 
	AttackFail = 0,
	//대시는 10으로
	DashPerfect = 14, 
	DashSuccess = 11, 
	DashFail = 10, 
	DashCancel = 9
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Normal UMETA(DisplayName = "Normal Attack"),
	Dash UMETA(DisplayName = "Dash Attack")
};

//액션 정보
UENUM(BlueprintType)
enum class EJudgementAction : uint8
{
	Attack UMETA(DisplayName = "Attack Action"),
	Dash UMETA(DisplayName = "Dash Action")
};

//스테이지 랭크 타입
UENUM(BlueprintType)
enum class ERankType : uint8
{
	S UMETA(DisplayName = "S Rank"),
	A UMETA(DisplayName = "A Rank"),
	B UMETA(DisplayName = "B Rank"),
	C UMETA(DisplayName = "C Rank"),
	None UMETA(DisplayName = "None Rank")
};

//인풋 액션의 종류
UENUM(BlueprintType)
enum class EInputAction : uint8
{
	Dash UMETA(DisplayName = "Dash Action"),
	Move UMETA(DisplayName = "Move Action"),
	Look UMETA(DisplayName = "Look Action"),
	ArrowMove UMETA(DisplayName = "Arrow Move Action"),
	Attack UMETA(DisplayName = "Attack Action"),
	Quit UMETA(DisplayName = "Quit Action"),
	Esc UMETA(DisplayName = "Esc"),
	Debug_ToggleDebugMode UMETA(DisplayName = "Toggle Debug Mode"),
	Debug_AllPerfectMode UMETA(DisplayName = "Debug : All perfect mode"),
	Debug_InfinityPurificationMode UMETA(DisplayName = "Debug : Infinity purification mode"),
	Debug_InfinityHealthAndStamina UMETA(DisplayName = "Debug : Infinity health and stemina mode"),
	Debug_Invincibility UMETA(DisplayName = "Debug : Invincibility mode"),
	Debug_FixPlayer UMETA(DisplayName = "Debug : Fix player Mode"),
	Debug_InfinityDamage UMETA(DisplayName = "Debug : Infinity damage"),
	Debug_ViewBeatIndex  UMETA(DisplayName = "Debug : View BeatIndex Log"),
	Debug_InfinityRange UMETA(DisplayName = "Debug : Infinity range"),
	//튜토리얼 전용
	InputAction_W UMETA(DisplayName = "W key OnPressed"),
	InputAction_A UMETA(DisplayName = "A key OnPressed"),
	InputAction_S UMETA(DisplayName = "S key OnPressed"),
	InputAction_D UMETA(DisplayName = "D key OnPressed")
};

UENUM(BlueprintType)
enum class ETutorialKeyType : uint8
{
	W, A, S, D
};

//디버그 모드 종류
UENUM(BlueprintType)
enum class EDebugMode : uint8
{
	DebugMode UMETA(DisplayName = "Debug Mode"),
	AllPerfect UMETA(DisplayName = "All Perfect Mode"),
	InfinityPurification UMETA(DisplayName = "Infinity Purification Mode"),
	InfinityHealthAndStamina UMETA(DisplayName = "Infinity Health And Stamina"),
	Invincibility	UMETA(DisplayName = "Invincibility Mode"),
	FixPlayer UMETA(DisplayName = "Fix Player Mode"),
	InfinityDamage UMETA(DisplayName = "Infinity Damage"),
	ViewBeatIndex UMETA(DisplayName = "View Log BeatIndex"),
	InfinityRange UMETA(DisplayName = "Infinity Range")
};

//게임 오버 타입
UENUM(BlueprintType)
enum class EGameOverType : uint8
{
	GameClear_Normal UMETA(DisplayName = "Game Clear_Normal"),
	GameClear_Boss UMETA(DisplayName = "Game Clear_Boss"),
	GameFailed_PlayerDied UMETA(DisplayName = "Game Failed_Player Died"),
	GameFailed_PurificationFailed UMETA(DisplayName = "Game Failed_Purification Failed")
};

//메뉴 타입
UENUM(BlueprintType)
enum class EMenu : uint8
{
	Intro UMETA(DisplayName = "Intro"),
	Main UMETA(DisplayName = "Main Menu"),
	HowToPlay UMETA(DisplayName = "How To Play"),
	Credit UMETA(DisplayName = "Credit"),
	Reset UMETA(DisplayName = "Reset"),
	SelectStage UMETA(DisplayName = "Select Stage"),
	StageEnterConfirm UMETA(DisplayName = "Stage Enter Confirm"),
	Story UMETA(DisplayName = "Story")
};

//메뉴 타입
UENUM(BlueprintType)
enum class EApplyDamageType : uint8
{
	Always     UMETA(DisplayName = "Always Apply Damage"),       // 항상 대미지
	OnAttack   UMETA(DisplayName = "Only When Attacking"),       // 공격 중일 때만
	Never      UMETA(DisplayName = "Do Not Apply Damage")        // 대미지 없음
};

UENUM(BlueprintType)
enum class ECommonEnemyType : uint8
{
	None			UMETA(DisplayName = "Select Enemy Type!"),
	Dummy			UMETA(DisplayName = "Dummy"),
	Test_A			UMETA(DisplayName = "For Test - A"),
	Test_B			UMETA(DisplayName = "For Test - B"),
	Test_C			UMETA(DisplayName = "For Test - C"),
	Dash_Basic		UMETA(DisplayName = "Dash Enemy - Basic type"),
	Dash_Heavy		UMETA(DisplayName = "Dash Enemy - Heavy type"),
	Dash_DashSkill	UMETA(DisplayNAme = "Dash Enemy - Dash Skill type")
};

//스테이지 기록 구조체
USTRUCT(Atomic, BlueprintType)
struct FStageRecord
{
	GENERATED_USTRUCT_BODY()
public:
	FStageRecord() : bIsClear(false), BestScore(0), BestClearStrike(0), BestPurified(0), BestCombo(0), ProgressPercent(0.0f), Rank(ERankType::None),Attempt(0) {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsClear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BestScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BestClearStrike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BestPurified;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BestCombo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProgressPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERankType Rank;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Attempt;
};

//세팅 데이타 구조체
USTRUCT(Atomic, BlueprintType)
struct FSettingData
{
	GENERATED_USTRUCT_BODY()
public:
	FSettingData() : InputOffset(0), BgmVolume(10), SfxVolume(10), UIVolume(10), MetronomeVolume(10), GraphicQuality(1){}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BgmVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SfxVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UIVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MetronomeVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GraphicQuality;
};

//투사체에 대한 기본적인 정보를 담은 변수
USTRUCT(Atomic, BlueprintType)
struct FProjectileInformation
{
	GENERATED_USTRUCT_BODY()
public:
	FProjectileInformation() : StartPosition({ 0, 0, 0 }), ProjectileSpeed(0.0f), ProjectileAngle({ 0, 0, 0 }), ProjectileRange(0) {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ProjectileAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* TargetClass;
};

/**
 * 
 */
class UCISMA_API CommonTypes
{
public:
	CommonTypes();
	~CommonTypes();
};