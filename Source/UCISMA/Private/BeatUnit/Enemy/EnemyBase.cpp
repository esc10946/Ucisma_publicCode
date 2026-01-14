// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatUnit/Enemy/EnemyBase.h"
#include "BeatUnit/Enemy/EnemyDataTable.h"
#include "BeatUnit/Enemy/EnemyStatComp.h"
#include "BeatUnit/Enemy/Enemy_AIController.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Manager/StageManager.h"
#include "Manager/TutorialManager.h"
#include "NiagaraFunctionLibrary.h"
#include "LevelDesign/Camera_Map.h"
#include "Widget/Widget_EnemyHealthBar.h"
#include "Widget/ActorWidgetInterface.h"
#include <Components/ProgressBar.h>

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable>EnemyDataObject(TEXT("/Script/Engine.DataTable'/Game/Data/EnemyDataTable1.EnemyDataTable1'"));
	static ConstructorHelpers::FClassFinder<UWidget_EnemyHealthBar> HealthBarWidgetClass(TEXT("/Game/UI/HUD/WBP_EnemyHealthBar"));
	

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	EnemyStat = CreateDefaultSubobject<UEnemyStatComp>(TEXT("ENEMYSTAT"));

	//데이터 테이블과 오브젝트 연결*
	EnemyDataTable = EnemyDataObject.Object;
	Tags.Add(TEXT("PurificationTarget"));

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 35.0f);

	//체력바 위젯 컴포넌트 속성 설정
	HPBarWidget->SetupAttachment(RootComponent);
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidget->SetDrawSize(FVector2D(150, 15.0f));
	HPBarWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HPBarWidget->SetWidgetClass(HealthBarWidgetClass.Class);
	HPBarWidget->SetTickWhenOffscreen(true);

	// AI 컨트롤러 클래스, AI 자동빙의
	// 폰의 생성 및 AI 컨트롤러에 의한 빙의 시기를 결정
	// 폰이 AI에 의해 컨트롤될 때 기본으로 사용할 AI 컨트롤러 클래스
	AIControllerClass = AEnemy_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//나중에 정예몹 나오면 dataTable에서 값을 가져오게할것
	SetEnemyType(EEnemyType::Common);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CheckGround();
	StageManager = GetStageManager();
	if (StageManager->IsTutorialStage()) {
		TutorialManager = StageManager->GetTutorialManager();
	}

	//DataTable 
	LoadEnemyData();
	InitEnemyData();

	fade_material = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, fade_material);

	//Handler 추가
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnOverlapBegin);

	/*if (UUserWidget* Widget = HPBarWidget->GetUserWidgetObject())
	{
		// 체력 바인딩 로직 추가 가능
	}*/

	TArray<AActor*> CameraMaps;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera_Map::StaticClass(), CameraMaps);

	if (CameraMaps.Num() > 0)
	{
		UnitList_Manager = Cast<ACamera_Map>(CameraMaps[0]);
	}

	UWidget_EnemyHealthBar* HPBar = Cast<UWidget_EnemyHealthBar>(HPBarWidget->GetUserWidgetObject());
	if (HPBar)
	{
		HPBar->InitWidget(this);
	}
	else
	{
		LOG_DEBUG(Warning, TEXT("Can't Find HPBar"));
	}
	
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHealthBar();
	if(GetIsLiving())	LookAtPlayer();

	if (!GetWorld() || !GetOwner()) return;

	FVector CurrentLocation = GetActorLocation() + FVector(60.f, 0.f, 0.f);

	// 카메라 위치 가져오기
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	CameraLocation.X = 0;
	CameraLocation.Y = 0;

	FVector WidgetLocation = GetOwner()->GetActorLocation();
	WidgetLocation.X = 0;
	WidgetLocation.Y = 0;

	// 카메라를 바라보는 방향 계산
	FVector DirectionToCamera = (CameraLocation - WidgetLocation).GetSafeNormal();

	// 위젯의 Yaw(수평) 회전만 유지
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToCamera).Rotator();

	// 회전 설정
	HPBarWidget->SetWorldRotation(LookAtRotation);
	HPBarWidget->SetWorldLocation(CurrentLocation);

	if (bIsDashing)
	{
		if (HasDashFx && DashTimeElapsed <= 0.0001f)
		{
			SpawnDashFx(DashFx, DashFxNiagaraComponent, StartLocation, EndLocation, FVector(0.9f, 0.3f, 0.1f), 0.3f);
		}
		DashTimeElapsed += DeltaTime;
		float Alpha = DashTimeElapsed / DashDuration;

		FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
		// 대시 이동 처리
		//SetActorLocation(NewLocation, true);
		FHitResult Hit;
		GetRootComponent()->MoveComponent(NewLocation - GetActorLocation(), GetActorRotation(), true, &Hit);

		//충돌 여부 확인
		if (Hit.IsValidBlockingHit())
		{
			//대쉬 이펙트 중단
			if (DashFxNiagaraComponent != nullptr) 
			{
				DashFxNiagaraComponent->SetVisibility(false);  //막혔을 시 대쉬 이펙트 제거 
			}
		}
		
			// 대시 시간 종료 시 멈춤
		if (DashTimeElapsed >= DashDuration)
		{
			bIsDashing = false;
			DashTimeElapsed = 0.0f;
		}
	}
}

void AEnemyBase::ActiveUnit()
{
	Super::ActiveUnit();
	if (!GetCharacterMovement()) return;
	GetCharacterMovement()->GravityScale = 1.0f;
	//얘가 자꾸 플레이하면 아래로 떨어져서 중력막아놨다가 푸는 시점
	//버그 알아내면 바꾸자잇

	UnitList_Manager->RegisterUnit(this, GetBeatIndex(),GetQuaterBeatIndex());
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AEnemyBase::Charge()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	CanDash = false;

	if (PlayerCharacter)
	{
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector TargetVector = PlayerLocation - GetActorLocation();
		TargetVector.Z = 0.0f;  // 수직 방향 무시

		if (!TargetVector.IsNearlyZero())
		{
			StartLocation = GetActorLocation();
			FVector DashDirection = TargetVector.GetSafeNormal();  // 대시 방향 설정
			EndLocation = StartLocation + DashDirection * DashDistance;
			
			bIsDashing = true;
			DashTimeElapsed = 0.0f;
		}
		else
		{
			LOG_KHJ(Error, TEXT("Dash failed: Invalid target vector"));
		}
	}
}

//EnemyData에 있는 DataIndex행의 정보를 가져옴
FEnemyDataStruct* AEnemyBase::GetEnemyData()
{
	//나중에는 DataIndex 대신해서 다른 방식으로 구현할 것, 직관적이지 않음
	if (EnemyType == ECommonEnemyType::None)
	{
		return EnemyDataTable->FindRow<FEnemyDataStruct>(*FString::FromInt(DataIndex), TEXT("EnemyDataTable1"));
	}

	TArray<FEnemyDataStruct*> EnemyDataStructs;
	EnemyDataTable->GetAllRows(TEXT("EnemyData"), EnemyDataStructs);

	for (FEnemyDataStruct* EnemyData : EnemyDataStructs)
	{
		if (EnemyData && EnemyData->D_EnemyType == this->EnemyType)
		{
			return EnemyData;
		}
	}

	LOG_DEBUG(Warning, TEXT("EnemyData can't get"));

	return nullptr;
}


//가져온 EnemyData를 로컬 변수에 저장
void AEnemyBase::LoadEnemyData()
{
	FEnemyDataStruct* EnemyData = GetEnemyData();

	if (EnemyData) {
		this->ID = EnemyData->D_ID;
		this->EnemyStatData = EnemyData->D_EnemyStatData;
		this->EnemyDashData = EnemyData->D_EnemyStatData.DashData;
		this->EnemyAssetData = EnemyData->D_EnemyAssetData;
	}
}

void AEnemyBase::InitEnemyData()
{
	if (EnemyStat)
	{
		EnemyStat->MaxHealth = EnemyStatData.D_MaxHealth;
		EnemyStat->CurrentHealth = EnemyStatData.D_MaxHealth;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyStatData.D_Speed;
	}

	if (EnemyAssetData.SkelMesh)
	{
		FString AuraPath = TEXT("/Game/Assets/Aura/Enemy_Material_Aura_Red2.Enemy_Material_Aura_Red2");
		UMaterialInterface* Aura = LoadObject<UMaterialInterface>(nullptr, *AuraPath); 
		GetMesh()->SetOverlayMaterial(Aura);

		GetMesh()->SetSkeletalMesh(EnemyAssetData.SkelMesh);
		GetMesh()->SetRelativeScale3D(SkelMeshSize);
		GetMesh()->SetRelativeLocationAndRotation(MeshLocation, MeshRotation);
	}

	if (EnemyAssetData.Color) {
		GetMesh()->SetMaterial(0, EnemyAssetData.Color);
	}

	this->CanTakeNormal = EnemyStatData.TakeDamage_NormalAttack;
	this->CanTakeDash = EnemyStatData.TakeDamage_DashAttack;
	this->HasDashAbility = EnemyStatData.D_HaveDashAbility;
	this->ApplyDamageType = EnemyStatData.EnemyApplyDamageType;
	

	if (HasDashAbility) {
		this->DashCount = EnemyDashData.D_DashCount;
		this->CurDashCount = 0;

		this->DashDistance = EnemyDashData.D_DashDistance;
		this->DashDuration = EnemyDashData.D_DashBeat * StageManager->GetBeatPerSecond();

		this->HasDashFx = EnemyStatData.DashData.D_HasDashFx;
		this->DashFx = EnemyStatData.DashData.D_DashFx;
	}
}

void AEnemyBase::BeatUpdate(int QuaterIndex)
{
	if (QuaterIndex != 0 || bIsDashing) return;

	if (DashCount == CurDashCount) {
		CurDashCount = 0;
		Charge();
	}
	else {
		CurDashCount++;
	}
}

void AEnemyBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		if (OtherComp && OtherComp->ComponentHasTag(TEXT("Sword")))
		{
			return; // 충돌 이벤트 처리 종료
		}
		else if (OtherComp && OtherComp->ComponentHasTag(TEXT("HitRange"))) {
			//플레이어 공격 하는 타이밍
			//이후에 공격후 다른 기능 구현시 여기에 할것
			AttackPlayer(Player);
		}
	}
}

void AEnemyBase::AttackPlayer(APlayerCharacter* Player)
{
	switch (ApplyDamageType)
	{
	case EApplyDamageType::Always:
		Player->Take_HP(-EnemyStatData.D_Damage, this);
		break;
	case EApplyDamageType::OnAttack:
		if(bIsDashing) Player->Take_HP(-EnemyStatData.D_Damage, this);
		break;
	case EApplyDamageType::Never:
		break;
	default:
		break;
	}

}

void AEnemyBase::UpdateHealthBar()
{
	UWidget_EnemyHealthBar* HPBar = Cast<UWidget_EnemyHealthBar>(HPBarWidget->GetUserWidgetObject());
	HPBar->UpdateValue();
}

//플레이어에게 맞는 함수
void AEnemyBase::TakeDamagePlayer(float Damage, EJudgementResult Judgement, AActor* DamageCauser)
{
	LOG_KHJ(Display, TEXT("[Attack Log] Damage: %.1f | Judgement: %d | Causer: %s"),
		Damage,
		static_cast<int32>(Judgement),
		*DamageCauser->GetName()
	);
	HitReaction(Damage, Judgement);
}

void AEnemyBase::MoveOut()
{
	/*
	//x축기준  회전행렬 변환
	FRotator NewRotation = FRotationMatrix::MakeFromX(MoveDirection).Rotator();
	//카메라 이동방향 반대로 회전
	SetActorRotation(NewRotation);
	FVector CurrentLocation = GetActorLocation();  // 현재 위치
	FVector TargetLocation = CurrentLocation + (MoveDirection * GetCharacterMovement()->MaxWalkSpeed * GetWorld()->GetDeltaSeconds());  // 목표 위치 (이동 방향 * 속도)
	
	// 새로운 위치로 이동
	SetActorLocation(TargetLocation);
	*/
	TimeElapsed += 0.016f;

	fade_material->SetScalarParameterValue(FName("opacity"), 1 - TimeElapsed);

	// 일정 시간 동안 이동 후 멈추고 유닛을 비활성화
	if (TimeElapsed > 1.0f)  //1초후
	{
		GetWorld()->GetTimerManager().ClearTimer(DeathMoveTimer);  // 타이머 종료
		DeactiveUnit();  // 유닛 비활성화
	}
}
//밑에 바닥이 없으면 killz에 가기전에 삭제
void AEnemyBase::CheckGround()
{
	GetCharacterMovement()->GravityScale = 0.0f;

	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 1000); // 🔹 아래 방향으로 라인트레이스

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 🔹 자기 자신 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	if (!bHit)
	{
		Destroy();
		return;
	}

	FVector TargetLocation = HitResult.ImpactPoint + FVector(0, 0, 10); // 바닥 위 10 유닛
	SetActorLocation(TargetLocation);
}

void AEnemyBase::HitReaction(float DamageAmount, EJudgementResult Judgement)
{
	//EJudgementResult를 보면 대시와 공격을 10으로 구분
	const int JudgeModular = 10;
	int AttackJudge = static_cast<int32>(Judgement);
	int AttackFxNum = AttackJudge % JudgeModular - 1;

	if(AttackFxNum >= 0)	OnEnemyHitFx(AttackFxNum);

	//퍼펙트 행동으로 맞을 경우 combo 
	if (Judgement == EJudgementResult::DashPerfect ||
		Judgement == EJudgementResult::AttackPerfect) {
		StageManager->AddCombo(1);
		StageManager->AddScore(StageManager->GetNowCombo() * 100);
	}
	if (EnemyStat->DecreaseHp(DamageAmount))
	{
		StageManager->AddScore(100);
		//유닛 죽음처리
		LivingUnit();
		GetMesh()->SetOverlayMaterial(nullptr);
		FTimerHandle PurificationTimerHandle;
		//1초뒤 유닛 사라짐
		//GetWorld()->GetTimerManager().SetTimer(PurificationTimerHandle, this, &AEnemyBase::DeactiveUnit, 4.0f, false);
		/* 이거 죽으면 해당 방향으로 걸어나가는 기능인데 쓸지 않쓸지 모름 냅둘것
		ECamDirection Direction = GetCameraDirection();
		MoveDirection = FVector::ZeroVector;

		switch (Direction)
		{
		case ECamDirection::Forward:
			MoveDirection = FVector(-1, 0, 0);
			break;
		case ECamDirection::Backward:
			MoveDirection = FVector(1, 0, 0);
			break;
		case ECamDirection::Left:
			MoveDirection = FVector(0, 1, 0);
			break;
		case ECamDirection::Right:
			MoveDirection = FVector(0, -1, 0);
			break;
		default:
			break;
		}
		*/
		if (AController* AIController = GetController())
		{
			AIController->UnPossess();
			//LOG_SGS_SCREEN(3.f, TEXT("%s"), *MoveDirection.ToString());

		}
		//메쉬와 캡슐 노콜리전화
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//벡터방향 정규화
		//MoveDirection = MoveDirection.GetSafeNormal();
		//1프레임마다 이동하도록 호출
		GetWorld()->GetTimerManager().SetTimer(DeathMoveTimer, this, &AEnemyBase::MoveOut, 0.016f, true);
		
		/*
		//움직이지 못함
		GetCharacterMovement()->SetMovementMode(MOVE_None);

		//회전 못함
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = false;
		*/

		const int PurificationCount = 1;
		StageManager->IncreasePurificationCount(PurificationCount);

		//유닛이 죽을 때 출력했던 대쉬 이펙트 제거
		if (DashFxNiagaraComponent != nullptr)
		{
			DashFxNiagaraComponent->SetVisibility(false);  
		}
		
		//DeactiveUnit(); //on death
	}
}

AStageManager* AEnemyBase::GetStageManager()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		// 첫 번째 StageManager를 가져옴
		return Cast<AStageManager>(FoundActors[0]);
	}
	LOG_KHJ(Error, TEXT("해당 스테이지에 stagemanager가 없음"));
	return nullptr;
}

void AEnemyBase::LookAtPlayer()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return;

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector EnemyLocation = GetActorLocation();

	FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();
	DirectionToPlayer.Z = 0.0f;

	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
	SetActorRotation(LookAtRotation);
}

ECamDirection AEnemyBase::GetCameraDirection()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACamera_Map::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ACamera_Map* CameraMap = Cast<ACamera_Map>(FoundActors[0]);
		if (CameraMap)
		{
			return CameraMap->GetDirection();
		}
		else
		{
			LOG_KHJ(Error, TEXT("카메라맵 못찾음"));
		}
	}
	else
	{
		LOG_KHJ(Error, TEXT("카메라맵 없음"));
	}
	return ECamDirection::Start;
}

void AEnemyBase::OnEnemyHitFx(int index) {
	if (NiagaraSystemAssets.IsValidIndex(index)) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraSystemAssets[index],
			GetActorLocation(),
			GetActorRotation()
		);
	}
}

//AttackType에 따라 데미지를 받는지 안받는지를 리턴 해줌
bool AEnemyBase::IsAvailableForAttack(EAttackType AttackType)
{
	if (AttackType == EAttackType::Normal) {
		return EnemyStatData.TakeDamage_NormalAttack;
	}
	else if (AttackType == EAttackType::Dash) {
		return EnemyStatData.TakeDamage_DashAttack;
	}
	else
	{
		LOG_KHJ(Error, TEXT("맞지않는 데미지 타입"));
		return false;
	}

}

bool AEnemyBase::IsInView() {
	return bInView;
}

void AEnemyBase::SetbInView(bool bIsInView)
{
	this->bInView = bIsInView;
}

void AEnemyBase::SetCommonEnemyType(ECommonEnemyType NewEnemyType)
{
	EnemyType = NewEnemyType;
}

void AEnemyBase::SpawnDashFx(UNiagaraSystem* Fx, UNiagaraComponent*& NiagaraComponent, const FVector& StartVector, const FVector& EndVector, FVector FxScaleMultiplier, float EffectLocationAlpha)
{
	if (Fx)
	{
		// 대시 방향 계산
		FVector DashDirection = (EndVector - StartVector).GetSafeNormal(); // 대쉬 방향을 정규화 벡터로 변환
		FRotator DashRotation = DashDirection.Rotation(); // 정규화 벡터로 회전값 계산
		// 이펙트의 위치 조정 (시작점과 끝점 사이)
		FVector AdjustedLocation = FMath::Lerp(StartVector, EndVector, EffectLocationAlpha);
		// 이펙트 스폰
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Fx, AdjustedLocation,	DashRotation);
		if (NiagaraComponent)
		{
			float DashLengh = FVector::DistXY(StartVector, EndVector);
			float DefaultFxLength = 100.0f;
			//목표 거리와 표준 이펙트 길이를 비교하여 이펙트 크기 설정
			float FxScaleX = DashLengh / DefaultFxLength;
			FVector DashScale(FxScaleX * FxScaleMultiplier.X, FxScaleMultiplier.Y, FxScaleMultiplier.Z);
			NiagaraComponent->SetVariableVec3(FName("MeshScale"), DashScale);
		}
	}
}
