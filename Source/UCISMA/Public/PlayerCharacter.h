// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Manager/BeatManager.h"
#include "Manager/StageManager.h"
#include "Widget/Widget_Fadeout_HP.h"
#include "LevelDesign/Camera_Map.h"
#include "NiagaraSystem.h" 
#include "Components/SphereComponent.h"
#include "BeatUnit/BeatUnitInterface.h"
#include "System/CommonTypes.h"
#include "PlayerCharacter.generated.h"



//스테이트 디스패쳐
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatsChanged, float, CurrentHealthPercent, float, CurrentStaminaPercent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, int, Player_Current_HP, int, changer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, int, Player_Current_HP, float, Player_Current_Health);

class UInputMappingContext;
class UInputAction;
class ABeatManager;
class ACamera_Map;
class ABeatUnitCharacter_Enemy;
class AStageManager;
struct FInputActionValue;


UENUM(BlueprintType)
enum class EInputMode : uint8
{
	WASDMovement,
	ArrowKeyMovement
};

UCLASS(config = Game)


class UCISMA_API APlayerCharacter : public ACharacter
{
public:

	//체력 변화 델리게이트(To Widget_HP)
	FOnHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStateChanged OnStateChanged;


	GENERATED_BODY()

	/** MappingContext */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_PlayerCharacter_PC;

	//인풋 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TMap<EInputAction, UInputAction*> InputActions;

	//공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Sword_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AActor> UI_Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* DynamicMaterial0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* DynamicMaterial1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* DynamicMaterial3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Judge")
	TEnumAsByte<EJudgementResult> notejudge;


	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* CircleEffectComponent; // CircleEffect를 관리할 변수 선언

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* PerfectDashComponent; // DashEffect를 관리할 변수 선언

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* HitEffectComponent; // HitEffect를 관리할 변수 선언

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* HealEffectComponent; // HealingEffect를 관리할 변수 선언

	float HealEffectOpacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor AttackColor;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BlockWidgetClass;

	UPROPERTY()
	UUserWidget* BlockWidgetInstance; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
	float Arrow_Width = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
	float Arrow_Length = 2.0f;


public:
	APlayerCharacter();
	//대쉬함수
	UFUNCTION()
	void Dash();
	//공격함수
	UFUNCTION(BlueprintCallable, Category = "Mobile Actions")
	void Attack();
	//게임종료 함수
	void QuitGame();
	//움직임 함수
	void Move(const FInputActionValue& Value);
	//화살표 인풋함수
	void ArrowMove(const FInputActionValue& Value);

	//방향전환 함수
	void Look(const FInputActionValue& Value);
	//게임중단 함수
	void PauseGame();
	//플레이어를 현위치 기준으로 카메라에 부착하는 함수
	UFUNCTION()
	void AttachToCamera();
	UFUNCTION()
	void DetachFromCamera();
	//공격범위 설정 함수
	void SetPlayerScale(float Attack_Range, float Dash_Range);
	//플레이어 조작 여부를 변경하는 함수
	void ActivateInput(bool bActivation);
	//beat에 맞춰 플레이어 상태 변경함수
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetPlayerState(float PlayerScale, float AttackRange, float DashRange, float ArrowFX);
	//벽으로 부착된 상태에서 Detach하는 함수
	void Detach_wall();
	UFUNCTION(BlueprintCallable)
	void SetCanDash(bool bCan);

	UFUNCTION(BlueprintCallable)
	float GetPlayerAttackRange() { return player_attack_range; }
	UFUNCTION()
	AStageManager* GetStageManager() { return StageManager; }



	//-------모바일---------
	// 
	//움직임 함수
	void Mobile_Move(const FInputActionValue & Value);
	UFUNCTION(BlueprintCallable, Category = "Mobile Actions")
	void Mobile_Dash();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();


public:
	virtual void Tick(float DeltaTime) override;

public:

	//count값에 따라 체력 증감
	UFUNCTION(BlueprintCallable)
	void Take_HP(int count, AActor* DamageCauser);


	//--플레이어 상태값--
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage; //플레이어 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage_Base; //플레이어 공격력 베이스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage_Perfect_Multiplier; // 퍼펙트 공격력 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage_Great_Multiplier; // 그레이트 공격력 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage_Good_Multiplier; // 굳 공격력 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Attack_Damage_Normal_Multiplier; // 노멀 공격력 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int Player_Current_HP; //플레이어 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int Player_MAX_HP = 3; //플레이어 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Current_Stamina; //플레이어 현재 스테미너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_MAX_Stamina = 100.0f; //플레이어 최대 스테미너

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_Current_Health = 0.0f; //플레이어 현재 회복수치 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Player_MAX_Health = 100.0f; //플레이어 최대 회복수치

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Dash_Stamina = 25.0f; //플레이어 대쉬 스테미너 소모량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	float Dash_Power = 1500.0f; //플레이어 대쉬 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float scale_indash = 3.5f; //플레이어 대쉬 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bCanDash = true; //플레이어 대쉬 값

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsInvincibility;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIgnoreInput;

	UPROPERTY(BlueprintReadOnly, Category = "Bis")
	bool bIsDashing = false; //현재 대쉬상태


	//히트박스 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* HitRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* ObstacleDetectionSphere;
	//공격범위 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UStaticMeshComponent* Attack_Round;

	//hp 피드백 UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UWidget_Fadeout_HP> WidgetClass;

	//대쉬 위치 보간 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float Adjust_Dash_Location = 0.3f;


	//BPM에 따른 최종 계수 값 bpm_speed = BPM값 / bpm_divide (기본값 : 1.0f)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speed")
	float bpm_speed = 1.0f; 
	//bpm값을 나눌 평균값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float bpm_divide = 90.0f;

	//플레이어 기본 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float Player_Default_Speed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float Player_Horizontal_Speed = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float Player_Vertical_Speed = 1.1f;	
	//플레이어가 벽에 attach 여부
	bool player_attach_wall = false;
	//입력여부(true:입력 false:미입력)
	bool bHasInput = false;




	//beat초
	float beatpersecond = 0.0f;
private:

	EInputMode CurrentInputMode = EInputMode::WASDMovement;


	//대쉬 실패시, 망토 색 변경(빨강)
	void OnDashFail_ChangeCape();
	//망토색 원래대로 돌림
	void Rest_ChanageCape();
	//망토색 바뀐 여부(true:바뀜 false:안바뀜)
	bool bIsCapeRed = false;

	//체력 최신화 델리게이트 보냄 -> widget_HP로
	void Change_HP_UI(int count);
	//회복게이지 정하는 함수
	void SetPlayer_Health(float health);
	//플레이어 상태 초기화
	void Player_State_Init();
	//스테이저 매니저 찾는 함수
	void Find_StageManager();

	//카메라 플레이어에게 고정
	void FindView();
	//공격가능한지 여부
	bool IsAttacking = false;
	//스태미너 사용 함수
	bool UseStamina(float Amount);


	//--공격관련 함수 --
	//공격 판정 함수
	void AttackJudge(EJudgementResult judgenote);
	//게임 시작시 적 배열 저장
	void Find_Enemy();
	//공격가능한 적인지 확인(중복인지 파악하는 함수)
	bool Check_Hit_Enemy(ABeatUnitCharacter_Enemy* Enemy);
	//오버랩된 애들중에 공격가능한 Enemy들만 골라서 enemy_area에 추가
	void Refresh_enemy_area_Enemies();
	//타깃으로 두거나 대미지를 줄수있는 지 파악하는 함수
	bool CheckCanTargetEnemy(ABeatUnitCharacter_Enemy* Enemy, EAttackType attack);
	//공격후 hit list에서 제거
	void OnAttackCompleted();
	//피격시 머터리얼 반투명함수
	void Event_OnHit();
	//피격 후 머터리얼 재복구함수
	void ResetOpacity();
	//공격 애니메이션 속도 정하는 함수
	void Set_Montage_Speed(UAnimMontage* Montage, float goalduration);

	void ApplyDamage(float Damage, AActor* Target, EAttackType AttackType);


	//bpm마다 스테미너 회복(스테이지매니저로부터 비트인덱스마다 호출받음)
	UFUNCTION()
	void BPM1_HP(int QuaterBeatIndex);


	//--FX관련함수--
	//논타겟팅시 원형 FX 생성 함수
	void SpawnCircleEffect(EJudgementResult judge);
	// DashEffect 생성 함수
	void SpawnDashEffect(const FVector& StartLocation, const FVector& EndLocation, int judge);


	//장애물 마테리얼 변경함수(맵과 겹쳐질 시)
	void Obstacle_SetMaterial(UPrimitiveComponent* Component, UMaterialInterface* NewMaterial);
	//플레이어의 앞 장애물 여부
	bool IsObstacleInFront();
	//플레이어 마테리얼 적용함수
	void Set_Material();
	//hp 피드백 ui 찾는 함수
	void Find_Fadeout_UI();
	//체력 증감에 따라 페이드아웃 ui 출력
	void FeedBack_Fadeout(int count);


	UMaterialInstanceDynamic* arrowMat;
	//---오버랩관련 함수----
	UFUNCTION()
	void PlayerMesh_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnObstacleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnObstacleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void Player_Attack_Round_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void Player_Attack_Round_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//검오버랩
	UFUNCTION()
	void OnSwordMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//카메라맵 클래스 변수선언
	ACamera_Map* CamActor;
	//스테이지 매니저 인스턴스(전방선언)
	AStageManager* StageManager;
	//비트매니저 클래스 변수선언
	ABeatManager* BeatManager;
	//페이드아웃 UI 변수선언
	UWidget_Fadeout_HP* FadeoutWidget;
	//튜토리얼 매니저 인스턴스
	class UTutorialManager* TutorialManager;


	//--플레이어 State 변수--
	//공격범위
	float player_attack_range = 12.0f;
	//타겟팅시 arrow 생성 최대 최소 거리
	float player_arrow_maxdistance;
	float player_arrow_mindistance;

	//--대쉬 관련 변수--

	float DashElapsedTime = 0.0f; //대쉬진행시간
	float DashDuration = 0.0f; //대쉬 얼마나 지속하는지 시간
	FVector StartLocation; //시작위치
	FVector EndLocation; //도착위치

	//대쉬 거리
	float MaxDash;
	//마우스 위치
	float CursorDash;

	UNiagaraComponent* Dash_NiagaraComp = nullptr; 	//대쉬 나이아가라컴포넌트

	//마우스 커서에 따라 회전하는 값
	FRotator mousecursor_rotation;
	//회전가능한지 여부
	bool canrotation = true;
	//공격모션가능한지 여부(true일경우 가능, false일경우 불가능)
	bool can_attackmotion = false;

	//타겟팅 화살표 액터
	AActor* ArrowActor;


	//적 클래스 배열
	TArray<ABeatUnitCharacter_Enemy*> EnemyArray;
	//타겟된 적 리스트
	TArray<ABeatUnitCharacter_Enemy*> hit_list;
	//오버랩된 적 리스트
	TArray<ABeatUnitCharacter_Enemy*> enemy_area;

	//오버랩된 적 리스트 + 타게팅에서 제외된 적 포함
	TSet<ABeatUnitCharacter_Enemy*> OverlappingEnemies;

	//피격 색깔 변경 가능 여부(true일때만 hp 감소)
	bool Can_ChangeMaterial = true; 
	//타이머핸들 Event_OnHit함수용
	FTimerHandle TimerHandle;


	//대쉬용 루트컴포넌트 캐스팅 변수
	UPrimitiveComponent* TopComponent;

	//플레이어 벽겹침 판정변수
	bool overlap_wall = false;
	bool overlap_obstacle = false;
	//플레이어 벽겹침 컴포넌트
	UPROPERTY()
	UPrimitiveComponent* OverlappingComponent;
	//게임오버 여부(중첩 사망 방지용)
	bool CanGameOver = true;
	

	//대쉬판정 변수
	EJudgementResult dashjudge;
	//공격판정 변수
	EJudgementResult AttackJudgement;

	//카메라 위치
	FVector CameraPosition;
	//플레이어컨트롤러
	APlayerController* PlayerController;



	//플레이어 탑뷰에서 다른 옵젝에 겹칠 경우 UI 출력
	void ShowWidgetOnBlockedView();

	//플레이어 방향에 따른 속도 설정함수
	void Set_PlayerSpeed();
	//타겟팅 시 화살표 생성
	void System_Target();

	void OnTutorial_W();
	void OnTutorial_A();
	void OnTutorial_S();
	void OnTutorial_D();
	void HandleTutorialKeyInput(ETutorialKeyType Key);
	void InitTutorialManager();

	//현재입력방향
	FVector2D MovementVector;

	//초기 HitRange 크기값(대쉬중 변경되는 크기값에 대한 리셋값)
	FVector initscale_hitrange;
	
	//플랫폼 여부
	bool IsPC; //true = PC falses = Mobile
};