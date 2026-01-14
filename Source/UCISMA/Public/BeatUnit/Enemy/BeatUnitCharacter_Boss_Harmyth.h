#pragma once

#include "UCISMA/UCISMA.h"
#include "LevelDesign/BossActionQueue.h"
#include "BeatUnit/BeatUnitCharacter_Enemy.h"
#include "LevelDesign/Boss_Harmyth_DataAsset.h"
#include "Components/ArroWComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Widget/Widget_BossHealthBar.h"
#include "Widget/Widget_ScreenFade.h"
#include "BeatUnitCharacter_Boss_Harmyth.generated.h"

UCLASS()
class UCISMA_API ABeatUnitCharacter_Boss_Harmyth : public ABeatUnitCharacter_Enemy
{
    GENERATED_BODY()

public:
    ABeatUnitCharacter_Boss_Harmyth();


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void ActiveUnit() override;
    virtual void DeactiveUnit() override;

public:
    // ======================[ 보스 상태 ]======================
    enum class EBossState { None, Idle, Groggy, Disappear };
    EBossState BossState;

    void SetBossState(EBossState state);
    void SetBossIndex(int Index);


    // ======================[ 보스 비트 관련 ]======================
    void InitBossData();

    virtual void BeatUpdate(int quarterIndex) override;

    void StartMoveForward();

    void StartMoveBackward();

    // ======================[ 보스 동작 및 AI ]======================
    void ProcessMoveQueue(float deltaTime);
    void ProcessWaitQueue(float deltaTime);
    void EnMoveQueue(const FBossMove& action);
    TArray<USkeletalMeshComponent*> GetMeshComponentByType(EBossMeshType meshType);

    // ======================[ 보스 체력 ]======================
    UFUNCTION(BlueprintCallable, Category = "Boss|Health")
    void IsHit(int amount);

    virtual void TakeDamagePlayer(float Damage, EJudgementResult Judgement, AActor* DamageCauser) override;

    UFUNCTION()
    void PurificationDamage();

    UFUNCTION()
    void OnDeath();

    // ======================[ 카메라 기능 ]======================
    UFUNCTION(BlueprintCallable)
    void InitCameraDistance(APlayerController* PC);

    UFUNCTION(BlueprintCallable)
    void ChasingCamera();

    // ======================[ 대미지 및 공격 ]======================
    UFUNCTION(BlueprintCallable)
    void OverlapHand(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool bIsHitTime();

    // ======================[ 보스 체력바 UI ]======================
    void InitHealthBar();

    void BindToStageManager(AStageManager* StageMgr);

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UWidget_ScreenFade> ScreenFadeWidgetClass;
    UWidget_ScreenFade* ScreenFadeWidget;

    // ======================[ 중간 시작 기능 ]======================
    void BossSetupStageSkip(int StartBeatIndex);

    bool CanDealDamageFromPart(UPrimitiveComponent* OverlappedComponent);
    EBossBodyPart GetPartTypeFromPriComponent(UPrimitiveComponent* Comp);

private:
    //=================Variable======================
    TArray<FBossMove> MoveQueue; // 실행할 동작 큐
    TArray<FBossMove> WaitQueue; // 앞에서 지연된 동작 큐

    // 메시별로 언제까지 동작이 예약되어 있는지
    TMap<USkeletalMeshComponent*, float> MoveEndTimeMap_Position;
    TMap<USkeletalMeshComponent*, float> MoveEndTimeMap_Rotation;


    UPROPERTY(EditAnywhere, Category = "Boss|Beat Settings")
    UBoss_Harmyth_DataAsset* beatDataAsset;


    UPROPERTY(EditAnywhere, Category = "Boss|HealthBar")
    TSubclassOf<UWidget_BossHealthBar> healthBarWidgetClass;

    UPROPERTY()
    UWidget_BossHealthBar* bossHealthBar;

    // === 메쉬 컴포넌트 ===
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* BossMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* HeadMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* HeadAuraMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* LeftHandMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* LeftHandAuraMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* RightHandMesh;
    UPROPERTY(VisibleAnywhere, Category = "Boss Mesh")
    USkeletalMeshComponent* RightHandAuraMesh;
    UPROPERTY(EditAnywhere, Category = "Arrow")
    UArrowComponent* FrontArrow;

    // === 콜리전 컴포넌트 ===
    UPROPERTY(VisibleAnywhere, Category = "Boss Collision")
    UCapsuleComponent* BodyCollision;
    UPROPERTY(VisibleAnywhere, Category = "Boss Collision")
    UCapsuleComponent* LeftHandCollision;
    UPROPERTY(VisibleAnywhere, Category = "Boss Collision")
    UCapsuleComponent* RightHandCollision;

    //=== 나이아가라 컴포넌트 ===
    UPROPERTY(VisibleAnywhere, Category = "Boss|niagara")
    UNiagaraComponent* LeftHandNiagaraComponent;
    UPROPERTY(VisibleAnywhere, Category = "Boss|niagara")
    UNiagaraComponent* RightHandNiagaraComponent;

    //mesh material variable
    UMaterialInstanceDynamic* BlackMaterial;
    UMaterialInstanceDynamic* EyeMaterial;
    UMaterialInstanceDynamic* AuraMaterial;

    //change meterial variable
    FTimerHandle FadeTimerHandle;

    float CurrentOpacity;

    // === 보스 체력 변수===
    UPROPERTY(EditAnywhere, Category = "Boss|Stats")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, Category = "Boss|Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Boss|Attack")
    float damage;

    // === 비트 관련 변수 ===
    UPROPERTY(VisibleAnywhere, Category = "Boss Index")
    int32 BossIndex;

    // ===맞았을때 머터리얼 fade강도
    UPROPERTY(EditAnywhere, Category = "Material|Flash")
    float FlashIntensity = 0.7f;

    float FlashElapsedTime = 0.f;
    bool bIsFlashing = false;

    // === boss InitPosition ===
    FVector CenterPosition;

    //FVector TargetLocation;               
    float MoveSpeed;

    float BeatDuration;

    double EnTime;
    double InterpTime;
    bool bisDance;

    //camera variable
    FVector CameraLocation;
    FRotator CameraRotation;
    FVector CamToBoss;
    bool bIsCamToBossInitialized;

    class APlayerCharacter* PlayerCharacter;

    const float CurrentRadius = 0.0f;
    const float TargetRadius = 10.0f;

    UPROPERTY(VisibleAnywhere, Category = "PostProcess")
    UPostProcessComponent* PostProcessComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Material)
    UMaterialInterface* PostProcessMaterial;
    UMaterialInstanceDynamic* DynamicMaterial;
    // 🔹 타이머 핸들 (효과 지속 시간 관리)
    FTimerHandle PostProcessTimerHandle;
    
    bool beingRunPost;
    bool beingDecayPost;

    AStageManager* CurrentStageManager = nullptr; // 🔹 현재 연결된 StageManager
    int32 BossHealth = 10;

    //나이아가라 컴포넌트를 바꾸는 함수, 바꾸는데 성공하면 true를 반환함
    bool SetNiagaraComponent(USkeletalMeshComponent* Mesh, float time);

    void Die();
    void PlayScreenFadeEffect();  
    void OnBossKillFailed();
    bool IsBossStillAlive();

    void InitMaterial();
    void SetFade(int Value);
    void FadeInMesh();

    void ActivePostProcess();
    void UpdatePostProcessEffect();
    void DeactivePostProcess();

    void OnHitFlashEffect();
    void UpdateFlashFade(float delta);
};

