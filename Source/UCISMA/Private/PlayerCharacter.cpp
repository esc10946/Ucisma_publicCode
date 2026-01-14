// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "UObject/EnumProperty.h" 
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/StageManager.h"
#include "Manager/BeatManager.h"
#include "Manager/TutorialManager.h"
#include "GameFramework/Actor.h"
#include "Manager/UIManager.h"
#include "Widget/Widget_BeatBar.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "BeatUnit/Enemy/BeatUnitCharacter_Boss_Harmyth.h"
#include "BeatUnit/BeatUnitCharacter_Enemy.h"
#include "Engine/DamageEvents.h"
#include "LevelDesign/Camera_Map.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Core/MyGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widget/Widget_Fadeout_HP.h"
//#include "Framework/Application/SlateApplication.h"

#define PERFECT_ATTACK_SCORE 100
#define GREAT_ATTACK_SCORE 50
#define GOOD_ATTACK_SCORE 25
#define RECOVER_SCORE 500




// Sets default values
APlayerCharacter::APlayerCharacter()
{

	GetCapsuleComponent()->InitCapsuleSize(42.f, 42.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = Player_Default_Speed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->MaxAcceleration = 100000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 100000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	PrimaryActorTick.bCanEverTick = true;
	Sword_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword_Mesh"));
	Sword_Mesh->SetupAttachment(GetMesh(), TEXT("R_Sword"));
	Sword_Mesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSwordMeshOverlap);

	HitRange = CreateDefaultSubobject<USphereComponent>(TEXT("HitRange"));
	HitRange->SetupAttachment(RootComponent);
	HitRange->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::PlayerMesh_BeginOverlap);

	ObstacleDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ObstacleDetectionSphere"));
	ObstacleDetectionSphere->SetupAttachment(RootComponent);
	ObstacleDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnObstacleOverlapBegin);
	ObstacleDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnObstacleOverlapEnd);

	//공격범위 설정
	Attack_Round = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attack_Round"));
	Attack_Round->SetupAttachment(RootComponent);
	Attack_Round->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::Player_Attack_Round_BeginOverlap);
	Attack_Round->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::Player_Attack_Round_EndOverlap);

	PerfectDashComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PerfectDashComponent"));
	PerfectDashComponent->SetupAttachment(GetMesh());

	CircleEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CircleEffectComponent"));
	CircleEffectComponent->SetupAttachment(GetMesh());
	CircleEffectComponent->SetAbsolute(false, true, false);
	CircleEffectComponent->SetWorldRotation(FRotator(0.f, 90.f, 0.f)); // Y축 90도 회전

	HealEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HealEffectComponent"));
	HealEffectComponent->SetupAttachment(GetMesh());

	HitEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffectComponent"));
	HitEffectComponent->SetupAttachment(GetMesh());

	bIsInvincibility = false;
	bIgnoreInput = false;
	HealEffectOpacity = 1.0f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FindView();
	//Find_StageManager();
	Find_Fadeout_UI();
	Find_Enemy();
	Set_Material();
	Player_State_Init();
	InitTutorialManager();
}

void APlayerCharacter::Set_Material()
{
	UMeshComponent* MeshComponent = GetMesh();
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComponent is null."));
		return;
	}

	// 슬롯 0
	UMaterialInterface* Material0 = MeshComponent->GetMaterial(0);
	if (Material0)
	{
		DynamicMaterial0 = UMaterialInstanceDynamic::Create(Material0, this);
		if (DynamicMaterial0)
		{
			UE_LOG(LogTemp, Warning, TEXT("success 0"));
			MeshComponent->SetMaterial(0, DynamicMaterial0);
		}
	}
	else
	{
		//1번슬롯 비었음 
	}

	// 슬롯 1
	UMaterialInterface* Material1 = MeshComponent->GetMaterial(1);
	if (Material1)
	{
		DynamicMaterial1_2 = UMaterialInstanceDynamic::Create(Material1, this);
		if (DynamicMaterial1_2)
		{
			UE_LOG(LogTemp, Warning, TEXT("success 1"));
			MeshComponent->SetMaterial(1, DynamicMaterial1_2);
			MeshComponent->SetMaterial(2, DynamicMaterial1_2);

		}
	}
	else
	{
		//2번 슬롯 비었음 
	}

	UMaterialInterface* Material3 = MeshComponent->GetMaterial(3);
	if (Material3)
	{
		DynamicMaterial3 = UMaterialInstanceDynamic::Create(Material3, this);
		if (DynamicMaterial3)
		{
			UE_LOG(LogTemp, Warning, TEXT("success 3"));
			MeshComponent->SetMaterial(3, DynamicMaterial3);
		}
	}
	else
	{
		//3번 슬롯 비었음 
	}
}


//적 배열 찾기
//Todo: Enemy, Boss만 선별하기
void APlayerCharacter::Find_Enemy() {
	EnemyArray.Empty();

	// BeatUnitInterface 클래스를 가진 모든 액터를 찾을 배열
	TArray<AActor*> FoundActors;

	// 월드에서 ABeatUnitCharacter_Enemy 클래스를 가지거나 자식 액터 검색
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABeatUnitCharacter_Enemy::StaticClass(), FoundActors);

	// Boss , PurificationTarget 태그가진 애들만 남김
	FoundActors = FoundActors.FilterByPredicate([](AActor* Actor) {
		return Actor && (Actor->ActorHasTag(FName("Boss")) || Actor->ActorHasTag(FName("PurificationTarget")));
		});



	// FoundActors 배열에서 BeatUnitInterface로 캐스팅 후 EnemyArray에 추가
	for (AActor* Actor : FoundActors)
	{
		ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(Actor);
		if (Enemy)
		{
			EnemyArray.Add(Enemy);
		}
	}


	//UE_LOG(LogTemp, Warning, TEXT("Found %d enemies."), EnemyArray.Num());
}

//hp 피드백 페이드아웃 함수
void APlayerCharacter::Find_Fadeout_UI() {
	if (WidgetClass)
	{
		// 위젯 생성
		FadeoutWidget = CreateWidget<UWidget_Fadeout_HP>(GetWorld(), WidgetClass);

		if (FadeoutWidget)
		{
			// 뷰포트에 추가
			FadeoutWidget->AddToViewport();
		}
	}
}

//스테이지 매니저 찾는 함수
void APlayerCharacter::Find_StageManager() {
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(World, AStageManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		// 첫 번째 StageManager를 가져옴
		StageManager = Cast<AStageManager>(FoundActors[0]);
		if (StageManager)
		{
			// BeatManager에 접근 가능
			StageManager->BeatUpdateListeners.AddDynamic(this, &APlayerCharacter::BPM1_HP);

			float BPMfloat = float(StageManager->GetBpm());
			bpm_speed = BPMfloat / bpm_divide; //Todo:수치 조정
			beatpersecond = StageManager->GetSecondPerBeat();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No StageManager found in the level."));
	}
}

void APlayerCharacter::Set_PlayerSpeed() {
	if (bHasInput && CamActor) {
		// 플레이어 이동 방향 설정
		ECamDirection PlayerDirection = ECamDirection::Start;
		if (FMath::Abs(MovementVector.X) > FMath::Abs(MovementVector.Y))
		{
			// X축 이동이 더 큰 경우 
			PlayerDirection = MovementVector.X > 0 ? ECamDirection::Right : ECamDirection::Left;
		}
		else if (FMath::Abs(MovementVector.Y) > 0.1f)
		{
			// Y축 이동이 더 큰 경우 
			PlayerDirection = MovementVector.Y > 0 ? ECamDirection::Forward : ECamDirection::Backward;
		}

		// CamActor의 방향과 PlayerDirection 비교
		if (CamActor && CamActor->GetDirection() == PlayerDirection)
		{
			// 방향이 일치할 경우 속도 증가(가로 세로 비율에 맞춰 이속 증가)
			if (PlayerDirection == ECamDirection::Right || PlayerDirection == ECamDirection::Left)
			{
				GetCharacterMovement()->MaxWalkSpeed = Player_Default_Speed * Player_Horizontal_Speed * bpm_speed;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = Player_Default_Speed * Player_Vertical_Speed * bpm_speed;
			}
		}
		else
		{
			// 방향이 일치하지 않을 경우 기본 속도 설정
			GetCharacterMovement()->MaxWalkSpeed = Player_Default_Speed * bpm_speed;
		}
	}
}

void APlayerCharacter::System_Target() {
	canrotation = true;
	Refresh_enemy_area_Enemies();

	if (!enemy_area.IsEmpty()) {
		can_attackmotion = true;
		canrotation = false;
		ACharacter* TargetEnemy = nullptr;
		float MinDistance = FLT_MAX;

		for (ABeatUnitCharacter_Enemy* Enemy : enemy_area)
		{
			ACharacter* EnemyActor = Cast<ACharacter>(Enemy);

			if (EnemyActor)
			{
				float Distance = FVector::Dist(GetActorLocation(), EnemyActor->GetActorLocation());
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					TargetEnemy = EnemyActor;
				}

			}
		}

		if (TargetEnemy)
		{
			FVector playerlocation = GetActorLocation(); //플레이어 위치 
			FVector enemylocation = TargetEnemy->GetMesh()->GetComponentLocation(); //적 위치
			FVector Direction = enemylocation - playerlocation; //플레이어에서 적까지의 방향벡터
			FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator(); //방향벡터를 회전값으로, 플레이어 위치에서 적으로 향하는 방향값(rotation)
			FVector NormalizeDirection = Direction.GetSafeNormal(); //방향벡터를 정규화(1)

			TargetRotation.Pitch = 0.0f; //z축회전값은 0으로
			SetActorRotation(TargetRotation); //플레이어 적에게 록온
			//----

			float Distance = FVector::Dist(playerlocation, enemylocation); //플레이어와 적 사이의 거리

			FVector objectLocation = playerlocation + NormalizeDirection * (Distance / 2.0f); //화살표 오브젝트를 둘 위치 선정(중간)

			float sizeFactor = Distance * 0.01f; //화살표 사이즈는 거리에 비례(1대1 비율)

			float UV = Arrow_Length;
			if (Distance >= 100.0f) {
				int32 Steps = FMath::FloorToInt((Distance - 100.0f) / 50.0f) + 1;
				UV += Steps * Arrow_Length;
			}

			FVector arrowLocation = FVector(playerlocation.X, playerlocation.Y, 0.01f);
			// **하나의 화살표만 유지**
			if (!ArrowActor) {
				// 화살표가 없으면 생성
				ArrowActor = GetWorld()->SpawnActor<AActor>(UI_Arrow, arrowLocation, TargetRotation);
				UStaticMeshComponent* MeshComp = ArrowActor->FindComponentByClass<UStaticMeshComponent>();
				arrowMat = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
				MeshComp->SetTranslucentSortPriority(-1);



			}
			else {
				// 이미 있으면 위치와 회전만 갱신
				ArrowActor->SetActorLocation(arrowLocation);
				ArrowActor->SetActorRotation(TargetRotation);
				ArrowActor->SetActorHiddenInGame(false);

			}
			if (arrowMat)
			{
				arrowMat->SetScalarParameterValue(FName("UV"), UV);  //화살표 개수
			}
			ArrowActor->SetActorScale3D(FVector(Arrow_Width, sizeFactor, 0.1f));
		}

	}
	else {
		if (can_attackmotion) {
			can_attackmotion = false;

			// **기존 화살표 삭제(Hidden)**
			if (ArrowActor) {
				ArrowActor->SetActorHiddenInGame(true);
			}
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//충돌보정용, 장애물에 의해 부드럽게 밀리는 효과
	FHitResult OutHit;
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.1f), GetActorRotation(), true, OutHit);
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.1f), GetActorRotation(), true, OutHit);

	System_Target();
	Set_PlayerSpeed();

	
	if (IsValid(HealEffectComponent) && HealEffectOpacity > -0.1f)
	{
		HealEffectOpacity -= DeltaTime * StageManager->GetBeatPerSecond();
		HealEffectComponent->SetVariableFloat(FName("OpacityValue"), HealEffectOpacity);
	}
	else
	{
		if (IsValid(HealEffectComponent)) {
			HealEffectComponent->DeactivateImmediate();
		}
	}
	
	//1LOG_SGS_SCREEN(3.f,TEXT("%s"), Can_ChangeMaterial ? TEXT("true") : TEXT("false"));

	//1LOG_SGS_SCREEN(3.f,TEXT("player_attach_wall: % s"), player_attach_wall ? TEXT("true") : TEXT("false"));
	if (IsPC && CurrentInputMode == EInputMode::WASDMovement) {
		if (PlayerController)
		{
			// Variable to store the hit result
			FHitResult HitResult;

			if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
			{
				// HitResult.Location gives you the world location under the cursor
				FVector CursorLocation = HitResult.Location;

				// Get the current location of the player character
				FVector CharacterLocation = GetActorLocation();

				// Calculate the direction from the character to the cursor location
				FVector Direction = CursorLocation - CharacterLocation;

				// Get the desired rotation to face the cursor
				mousecursor_rotation = FRotationMatrix::MakeFromX(Direction).Rotator();

				// Optionally, lock the pitch to 0 so the character only rotates on the yaw axis (2D rotation)
				mousecursor_rotation.Pitch = 0.0f;

				if (canrotation) {
					SetActorRotation(mousecursor_rotation);
				}
			}

		}
	}


//대쉬 조건문
	if (bIsDashing)
	{
		DashElapsedTime += DeltaTime;
		if (DashElapsedTime < DashDuration)
		{
			Can_ChangeMaterial = false;
			float Alpha = DashElapsedTime / DashDuration;

			FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
			//대쉬 전-후 적에 대한 블록상태 제거
			TopComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);

			FHitResult Hit;
			// MoveComponent를 사용하여 이동 및 충돌 처리
			GetRootComponent()->MoveComponent(NewLocation - GetActorLocation(), GetActorRotation(), true, &Hit);

			//DrawDebugPoint(GetWorld(), EndLocation, 10.0f, FColor::Red, false, 1.0f);
			//DrawDebugPoint(GetWorld(), NewLocation, 10.0f, FColor::Blue, false, 1.0f);

			// 충돌 확인
			if (Hit.IsValidBlockingHit()) {

				// 적 오브젝트 타입인지 확인
				if (Hit.Component->GetCollisionObjectType() == ECC_GameTraceChannel3)
				{
					//LOG_SGS_SCREEN(3.f, TEXT("%s"), Can_ChangeMaterial ? TEXT("true") : TEXT("false"));
				}
				else {

					bIsDashing = false;
					//대쉬가 막혔을 때 HitRange 콜리젼 축소 
					HitRange->SetWorldScale3D(initscale_hitrange);

					//대쉬 전-후 적에 대한 무시상태 제거
					TopComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
					if (Dash_NiagaraComp != nullptr) {
						Dash_NiagaraComp->SetVisibility(false);  //막혔을 시 대쉬 이펙트 제거 
					}
					if (PerfectDashComponent != nullptr)
					{
						PerfectDashComponent->Deactivate();

					}
					Can_ChangeMaterial = true;
				}
			}
		}
		else
		{
			HitRange->SetWorldScale3D(initscale_hitrange);
			bIsDashing = false;
			//대쉬 전-후 적에 대한 블록상태 제거
			TopComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
			if (PerfectDashComponent != nullptr) {
				PerfectDashComponent->Deactivate();
			}
			Can_ChangeMaterial = true;
		}
	}
	ShowWidgetOnBlockedView();

	// 벽과 장애물 둘 다 활성화된 상태에서만 HP 감소 실행
	if (overlap_wall && overlap_obstacle) {
		{
			if (IsObstacleInFront()) {

				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(OverlappingComponent);
				if (!MeshComp) {
					AActor* OverlappingActor = OverlappingComponent->GetOwner();
					MeshComp = OverlappingActor->FindComponentByClass<UStaticMeshComponent>();
				}
				Take_HP(-1, this);
				FString MaterialPath = TEXT("Material'/Game/Assets/SpawnPlane/spawn_title_material1.spawn_title_material1'");

				UMaterialInterface* NewMaterial = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
				if (NewMaterial) {
					Obstacle_SetMaterial(MeshComp, NewMaterial);
				}
				if (MeshComp) {
					MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
			}
		}
	}
}





//인풋 시스템
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//게임인스턴스를 캐스팅해서 플랫폼 bool변수값을 가져옴 IsPc = bIsPC
	UMyGameInstance* myinstance = Cast<UMyGameInstance>(GetGameInstance());
	IsPC = myinstance->bIsPC;
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Find_StageManager();
	//FString CurrentPlatform = FPlatformProperties::PlatformName();
	//LOG_SGS_SCREEN(3.f, TEXT("Current Platform: %s"), *CurrentPlatform);
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IMC_PlayerCharacter_PC != nullptr)
			{
				Subsystem->AddMappingContext(IMC_PlayerCharacter_PC, 0);
			}
			else
			{

			}
		}
		//바인딩
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
			if (IsPC) {

				LOG_SGS_SCREEN(3.f, TEXT("PC맵핑"));
				// dash
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Dash], ETriggerEvent::Started, this, &APlayerCharacter::Dash);
				// Moving
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Move], ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
				
				EnhancedInputComponent->BindAction(InputActions[EInputAction::ArrowMove], ETriggerEvent::Triggered, this, &APlayerCharacter::ArrowMove);

				// Looking
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Look], ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
				//나가기
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Quit], ETriggerEvent::Triggered, this, &APlayerCharacter::QuitGame);
				//공격
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Attack], ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
				//Esc
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Esc], ETriggerEvent::Triggered, this, &APlayerCharacter::PauseGame);

				//디버그모드들
				if (StageManager)
				{
					
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_ToggleDebugMode], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleDebugMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_AllPerfectMode], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleAllPerfectMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_InfinityPurificationMode], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleInfinityPurificationMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_InfinityHealthAndStamina], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleInfinityHealthAndStaminaMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_Invincibility], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleInvincibilityMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_FixPlayer], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleFixPlayerMode);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_InfinityDamage], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleInfinityDamage);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_ViewBeatIndex], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleViewBeatIndex);	
					EnhancedInputComponent->BindAction(InputActions[EInputAction::Debug_InfinityRange], ETriggerEvent::Triggered, StageManager, &AStageManager::ToggleInfinityRange);
				
				}
				else
				{
					LOG_DEBUG(Error, TEXT("StageManager Not Exitsted"));
				}

				if (StageManager->IsTutorialStage()) {
					EnhancedInputComponent->BindAction(InputActions[EInputAction::InputAction_W], ETriggerEvent::Triggered, this, &APlayerCharacter::OnTutorial_W);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::InputAction_A], ETriggerEvent::Triggered, this, &APlayerCharacter::OnTutorial_A);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::InputAction_S], ETriggerEvent::Triggered, this, &APlayerCharacter::OnTutorial_S);
					EnhancedInputComponent->BindAction(InputActions[EInputAction::InputAction_D], ETriggerEvent::Triggered, this, &APlayerCharacter::OnTutorial_D);
				}
			}
			else
			{
				//모바일용
				// Moving
				LOG_SGS_SCREEN(3.f, TEXT("모바일 이동"));
				EnhancedInputComponent->BindAction(InputActions[EInputAction::Move], ETriggerEvent::Triggered, this, &APlayerCharacter::Mobile_Move);
				//FSlateApplication::Get().SetGameIsFakingTouchEvents(false);
			}
		}
		else
		{
			//맵핑 불러오지 못함
		}
	

	}
	else {
		LOG_SGS_SCREEN(3.f,TEXT("플레이어컨트롤러 문제"));
	}
}

void APlayerCharacter::Mobile_Move(const FInputActionValue& Value) {
	if (bIgnoreInput)
	{
		return;
	}

	MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		/*
		// 입력값이 너무 작으면 무시
		if (MovementVector.IsNearlyZero())
		{
			return;
		}
		*/


		bHasInput = MovementVector.SizeSquared() > KINDA_SMALL_NUMBER;
		if (bHasInput && player_attach_wall)
		{
			Detach_wall();
			//1LOG_SGS_SCREEN(3.f, TEXT("벽에서 attach해제"));
		}

		// 이동 방향 벡터 계산
		FVector MoveDirection = (FVector(1, 0, 0) * MovementVector.Y) + (FVector(0, 1, 0) * MovementVector.X);
		MoveDirection.Normalize();

		// 캐릭터 회전 적용 (Yaw 회전만 변경)
		FRotator NewRotation = MoveDirection.Rotation();
		NewRotation.Pitch = 0.0f; // 상하 회전 제거
		NewRotation.Roll = 0.0f;
		SetActorRotation(NewRotation);
		//LOG_SGS_SCREEN(3.f, TEXT("New Rotation: %s"), *NewRotation.ToString());

		// 이동 적용
		AddMovementInput(MoveDirection, 1.0f);
	}
}

//이동
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	CurrentInputMode = EInputMode::WASDMovement;

	if (bIgnoreInput)
	{
		return;
	}
	
	MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{

		bHasInput = MovementVector.SizeSquared() > KINDA_SMALL_NUMBER;
		if (bHasInput && player_attach_wall)
		{
			FVector2D InputDir = MovementVector.GetSafeNormal();

			bool bIsMovingForward = false;
			// 카메라 방향에 따른 벡터 설정
			ECamDirection Direction = CamActor->GetDirection();
			switch (Direction)
			{
			case ECamDirection::Forward:
				bIsMovingForward = InputDir.Y > 0.8f;
				break;
			case ECamDirection::Backward:
				bIsMovingForward = InputDir.Y < -0.8f;
				break;
			case ECamDirection::Right:
				bIsMovingForward = InputDir.X > 0.8f;
				break;
			case ECamDirection::Left:
				bIsMovingForward = InputDir.X < -0.8f;
				break;
			default:
				bIsMovingForward = false;
				break;
			}
			if (bIsMovingForward) {
				Detach_wall();
				//1LOG_SGS_SCREEN(3.f, TEXT("Detach 해제"));
			}
		}

		AddMovementInput(FVector(1, 0, 0), MovementVector.Y);
		AddMovementInput(FVector(0, 1, 0), MovementVector.X);

	}
}

void APlayerCharacter::ArrowMove(const FInputActionValue& Value) {
	CurrentInputMode = EInputMode::ArrowKeyMovement;
	if (bIgnoreInput)
	{
		return;
	}

	MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		/*
		// 입력값이 너무 작으면 무시
		if (MovementVector.IsNearlyZero())
		{
			return;
		}
		*/


		bHasInput = MovementVector.SizeSquared() > KINDA_SMALL_NUMBER;
		if (bHasInput && player_attach_wall)
		{
			Detach_wall();
			//1LOG_SGS_SCREEN(3.f, TEXT("벽에서 attach해제"));
		}

		// 이동 방향 벡터 계산
		FVector MoveDirection = (FVector(1, 0, 0) * MovementVector.Y) + (FVector(0, 1, 0) * MovementVector.X);
		MoveDirection.Normalize();

		// 캐릭터 회전 적용 (Yaw 회전만 변경)
		FRotator NewRotation = MoveDirection.Rotation();
		NewRotation.Pitch = 0.0f; // 상하 회전 제거
		NewRotation.Roll = 0.0f;
		SetActorRotation(NewRotation);
		//LOG_SGS_SCREEN(3.f, TEXT("New Rotation: %s"), *NewRotation.ToString());

		// 이동 적용
		AddMovementInput(MoveDirection, 1.0f);
	}
}

//마우스 위치에 맞춰 방향 전환
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	 {
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

//게임 나가기
void APlayerCharacter::QuitGame()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

//대쉬 스킬
void APlayerCharacter::Dash() {
	if (!bCanDash)
	{
		return;
	}

	if (CurrentInputMode == EInputMode::WASDMovement) {
		if (bIgnoreInput)
		{
			return;
		}

		if (Player_Current_Stamina >= Dash_Stamina) {
			dashjudge = StageManager->CheckJudgement(EJudgementAction::Dash);
			if (dashjudge == DashFail) {
				OnDashFail_ChangeCape();
				StageManager->PlaySfx(ESfx::DashFailed);
			}
			if (dashjudge != DashCancel) {

				Player_Current_Stamina -= Dash_Stamina;
				if (!bIsDashing && (dashjudge == DashSuccess || dashjudge == DashPerfect)) {
					OnAttackCompleted();
					DashElapsedTime = 0.0f;
					DashDuration = beatpersecond / 4; //0.25beat

					StartLocation = GetActorLocation();
					FVector ForwardDirection = mousecursor_rotation.Vector();
					EndLocation = StartLocation + ForwardDirection * (Dash_Power * DashDuration);
					float MaxDashDistance = FVector::DistXY(StartLocation, EndLocation); // 실제 최대 대시 거리 계산
					MaxDash = MaxDashDistance; //최대 대시 거리 저장해둠
					// 마우스 위치 가져오기
					if (PlayerController) {
						FHitResult HitResult;
						if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult)) {
							FVector CursorLocation = HitResult.Location;

							float DistanceToCursor = FVector::DistXY(StartLocation, CursorLocation);
							if (DistanceToCursor < MaxDashDistance) {
								CursorLocation.Z = StartLocation.Z;
								EndLocation = CursorLocation;
							}
							// 디버그 라인 그리기
							//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 5.0f, 0, 2.0f);
						}
					}

					bIsDashing = true;
					HitRange->SetWorldScale3D(FVector(scale_indash, scale_indash, scale_indash)); //대쉬시 공격판정 넓힘

					if (dashjudge == DashPerfect) {
						Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Perfect_Multiplier;
						//SpawnDashEffect(StartLocation, EndLocation, 1);
						if (PerfectDashComponent != nullptr) {
							PerfectDashComponent->SetVariableFloat(FName("lifetime"), beatpersecond / 2);
							PerfectDashComponent->Activate();
						}
						StageManager->PlaySfx(ESfx::DashPerfect);
					}
					else if (dashjudge == DashSuccess) {
						Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Normal_Multiplier;
						SpawnDashEffect(StartLocation, EndLocation, 0);
						StageManager->PlaySfx(ESfx::DashNormal);
					}
					if (StageManager->IsTutorialStage()) {
						if (TutorialManager) {
							TutorialManager->OnTutorialDashInput(dashjudge);
						}
					}
				}
			}

		}
		//대쉬 게이지가 없는 경우
		else
		{
			OnDashFail_ChangeCape();
			StageManager->PlaySfx(ESfx::DashFailed);
		}
	}

	//arrow대쉬
	else {
		if (bIgnoreInput)
		{
			return;
		}

		LOG_SGS_SCREEN(3.f, TEXT("모바일대쉬실행"));

		if (Player_Current_Stamina < Dash_Stamina)
		{
			OnDashFail_ChangeCape();
			StageManager->PlaySfx(ESfx::DashFailed);
			return;
		}


		dashjudge = StageManager->CheckJudgement(EJudgementAction::Dash);
		if (dashjudge == DashCancel) return;

		Player_Current_Stamina -= Dash_Stamina;

		if (!bIsDashing && (dashjudge == DashSuccess || dashjudge == DashPerfect)) {
			OnAttackCompleted();
			DashElapsedTime = 0.0f;
			DashDuration = beatpersecond / 4; // 0.25beat

			StartLocation = GetActorLocation();
			FVector ForwardDirection = GetActorForwardVector();
			EndLocation = StartLocation + ForwardDirection * (Dash_Power * DashDuration);

			float MaxDashDistance = FVector::DistXY(StartLocation, EndLocation); // 최대 대시 거리 계산
			MaxDash = MaxDashDistance; // 저장

			bIsDashing = true;

			if (dashjudge == DashPerfect) {
				Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Perfect_Multiplier;
				if (PerfectDashComponent != nullptr) {
					PerfectDashComponent->SetVariableFloat(FName("lifetime"), beatpersecond / 2);
					PerfectDashComponent->Activate();
					StageManager->PlaySfx(ESfx::DashPerfect);
				}
			}
			else if (dashjudge == DashSuccess) {
				Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Normal_Multiplier;
				SpawnDashEffect(StartLocation, EndLocation, 0);
				StageManager->PlaySfx(ESfx::DashNormal);
			}
		}
		else
		{
			OnDashFail_ChangeCape();
			StageManager->PlaySfx(ESfx::DashFailed);
		}

	}
}

void APlayerCharacter::Mobile_Dash() {
	if (bIgnoreInput)
	{
		return;
	}

	LOG_SGS_SCREEN(3.f, TEXT("모바일대쉬실행"));

	if (Player_Current_Stamina < Dash_Stamina)
	{
		OnDashFail_ChangeCape();
		StageManager->PlaySfx(ESfx::DashFailed);
		return;
	}


	dashjudge = StageManager->CheckJudgement(EJudgementAction::Dash);
	if (dashjudge == DashCancel) return;

	Player_Current_Stamina -= Dash_Stamina;

	if (!bIsDashing && (dashjudge == DashSuccess || dashjudge == DashPerfect)) {
		OnAttackCompleted();
		DashElapsedTime = 0.0f;
		DashDuration = beatpersecond / 4; // 0.25beat

		StartLocation = GetActorLocation();
		FVector ForwardDirection = GetActorForwardVector();
		EndLocation = StartLocation + ForwardDirection * (Dash_Power * DashDuration);

		float MaxDashDistance = FVector::DistXY(StartLocation, EndLocation); // 최대 대시 거리 계산
		MaxDash = MaxDashDistance; // 저장

		bIsDashing = true;

		if (dashjudge == DashPerfect) {
			Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Perfect_Multiplier;
			if (PerfectDashComponent != nullptr) {
				PerfectDashComponent->SetVariableFloat(FName("lifetime"), beatpersecond / 2);
				PerfectDashComponent->Activate();
				StageManager->PlaySfx(ESfx::DashPerfect);
			}
		}
		else if (dashjudge == DashSuccess) {
			Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Normal_Multiplier;
			SpawnDashEffect(StartLocation, EndLocation, 0);
			StageManager->PlaySfx(ESfx::DashNormal);
		}
	}
	else
	{
		OnDashFail_ChangeCape();
		StageManager->PlaySfx(ESfx::DashFailed);
	}

}

void APlayerCharacter::OnTutorial_W()
{
	HandleTutorialKeyInput(ETutorialKeyType::W);
}

void APlayerCharacter::OnTutorial_A()
{
	HandleTutorialKeyInput(ETutorialKeyType::A);
}

void APlayerCharacter::OnTutorial_S()
{
	HandleTutorialKeyInput(ETutorialKeyType::S);
}

void APlayerCharacter::OnTutorial_D()
{
	HandleTutorialKeyInput(ETutorialKeyType::D);
}

void APlayerCharacter::HandleTutorialKeyInput(ETutorialKeyType KeyCode)
{
	if (TutorialManager) {
		TutorialManager->OnTutorialKeyInput(KeyCode);
	}
}

void APlayerCharacter::InitTutorialManager()
{
	//튜토리얼 스테이지면 가져오기
	if (StageManager->IsTutorialStage()) {
		TutorialManager = StageManager->GetTutorialManager();
		if (!StageManager->GetTutorialManager()) LOG_KHJ(Error, TEXT("GetTutorialManager() Error"));
		if (!TutorialManager) LOG_KHJ(Error, TEXT("tuto not connected"));
	}
}

void APlayerCharacter::SpawnDashEffect(const FVector& StartVector, const FVector& EndVector, int judge)
{
	if (DashEffect)
	{
		Dash_NiagaraComp = nullptr;

		if (IsPC) {
			FVector AdjustedLocation = FMath::Lerp(StartLocation, EndLocation, Adjust_Dash_Location);
			if (judge == 0) {
				if (CurrentInputMode == EInputMode::WASDMovement) {
					Dash_NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						DashEffect,
						AdjustedLocation,
						mousecursor_rotation
					);
				}
				else {
					FVector DashDirection = (EndVector - StartVector).GetSafeNormal(); // 정규화된 방향 벡터
					FRotator DashRotation = DashDirection.Rotation(); // 해당 벡터를 회전값으로 변환

					// 이펙트의 위치 조정 (시작점과 끝점 사이)
					Dash_NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						DashEffect,
						AdjustedLocation,
						DashRotation
					);
				}
			}


			if (Dash_NiagaraComp)
			{
				//현재 대시 거리 (마우스 커서 포함)(기존 MAXDash는 Dash 로직에서 계산해둠)
				float CurrentDashDistance = FVector::DistXY(StartLocation, EndLocation);

				//FX의 X 스케일 조정 (최소 0.1 | 최대 2.0)
				float ScaleFactor = Dash_Power / 3000.0f;  // 3000 기준으로 스케일 조정
				float NormalizedDistance = CurrentDashDistance / MaxDash;
				float ScaledX = FMath::Lerp(0.2f, 2.0f, FMath::Clamp(NormalizedDistance * ScaleFactor, 0.0f, 1.0f));
				FVector MeshScale(ScaledX, 0.5f, 1.0f); // y,z축은 유지 


				Dash_NiagaraComp->SetVariableVec3(FName("MeshScale"), MeshScale);
			}
		}

		else {
			Dash_NiagaraComp = nullptr;
			// 대시 방향 계산
			FVector DashDirection = (EndVector - StartVector).GetSafeNormal(); // 정규화된 방향 벡터
			FRotator DashRotation = DashDirection.Rotation(); // 해당 벡터를 회전값으로 변환

			// 이펙트의 위치 조정 (시작점과 끝점 사이)
			FVector AdjustedLocation = FMath::Lerp(StartVector, EndVector, Adjust_Dash_Location);

			// 이펙트 스폰
			if (judge == 0) {
				Dash_NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					DashEffect,
					AdjustedLocation,
					DashRotation
				);
			}

			if (Dash_NiagaraComp)
			{
				// 현재 대시 거리 계산
				float CurrentDashDistance = FVector::DistXY(StartVector, EndVector);

				// FX 크기 조정
				float ScaleFactor = Dash_Power / 3000.0f;
				float NormalizedDistance = CurrentDashDistance / MaxDash;
				float ScaledX = FMath::Lerp(0.5f, 2.0f, FMath::Clamp(NormalizedDistance * ScaleFactor, 0.0f, 1.0f));
				FVector MeshScale(ScaledX, 0.5f, 1.0f);

				Dash_NiagaraComp->SetVariableVec3(FName("MeshScale"), MeshScale);
			}
		}
	}
}


void APlayerCharacter::SpawnCircleEffect(EJudgementResult judge)
{
	if (CircleEffectComponent)
	{
		CircleEffectComponent->SetVariableFloat(FName("circle_bpm"), beatpersecond);
		FLinearColor mincolor;
		FLinearColor maxcolor;
		if (notejudge == EJudgementResult::AttackPerfect)
		{
			mincolor = FLinearColor(0.8f, 0.4f, 0.0f, 1.0f);
			maxcolor = FLinearColor(1.0f, 0.6f, 0.2f, 1.0f);
		}
		else if (notejudge == EJudgementResult::AttackGreat)
		{
			mincolor = FLinearColor(0.6f, 0.6f, 0.0f, 1.0f);
			maxcolor = FLinearColor(0.8f, 0.8f, 0.2f, 1.0f);
		}
		else if (notejudge == EJudgementResult::AttackGood)
		{
			mincolor = FLinearColor(0.0f, 0.6f, 0.0f, 1.0f);
			maxcolor = FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);
		}
		else if (notejudge == EJudgementResult::AttackNormal)
		{
			mincolor = FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);
			maxcolor = FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
		}
		CircleEffectComponent->SetVariableLinearColor(TEXT("circle_max_color"), maxcolor);
		CircleEffectComponent->SetVariableLinearColor(TEXT("circle_min_color"), mincolor);

		CircleEffectComponent->Activate(true);

		if (TutorialManager) {
			TutorialManager->OnTutorialAttackInput();
		}
	}
}

void APlayerCharacter::Attack() {
	if (bIgnoreInput)
	{
		return;
	}

	notejudge = StageManager->CheckJudgement(EJudgementAction::Attack);
	OnAttackCompleted();
	AttackJudge(notejudge);
	if (!IsAttacking && notejudge != AttackFail) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && can_attackmotion)
		{
			StageManager->PlaySfx(ESfx::AttackInRange);
			Set_Montage_Speed(AttackMontage, beatpersecond);
			
		}
		else if (AnimInstance && !can_attackmotion) {
			StageManager->PlaySfx(ESfx::AttackOutRange);
			SpawnCircleEffect(notejudge);
			Set_Montage_Speed(AttackWave, beatpersecond);
		}
	}
}

//카메라 고정
void APlayerCharacter::FindView() {
	//카메라 찾기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tag_Camera"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		// 태그를 가진 첫 번째 카메라를 가져옵니다
		AActor* CameraActor = FoundActors[0];
		CamActor = Cast<ACamera_Map>(FoundActors[0]);
		if (CameraActor)
		{
			// 플레이어 컨트롤러를 가져와서 뷰 타겟 변경
			if (PlayerController)
			{
				PlayerController->SetViewTargetWithBlend(CameraActor, 0.0f); // 1초 동안 부드럽게 전환

			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tag_Camera 태그를 가진 카메라를 찾을 수 없습니다."));
	}
}
//상태 초기화
void APlayerCharacter::Player_State_Init() {
	Player_MAX_HP = 3;
	Player_Current_HP = Player_MAX_HP;
	Player_MAX_Stamina = 100.0f;
	Player_Current_Stamina = Player_MAX_Stamina;
	Player_MAX_Health = 100.0f;
	Player_Current_Health = 0.0f;
	Player_Attack_Damage_Base = 100.0f;
	Player_Attack_Damage_Perfect_Multiplier = 1.0f;
	Player_Attack_Damage_Great_Multiplier = 0.75f;
	Player_Attack_Damage_Good_Multiplier = 0.5f;
	Player_Attack_Damage_Normal_Multiplier = 0.25f;

	SetPlayerScale(StageManager->GetPlayerRange(), StageManager->GetPlayerDashRange());
	CanGameOver = true;
	TopComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	//망토 머터리얼 설정
	float NormalizedStamina = FMath::Clamp(Player_Current_Stamina / Player_MAX_Stamina, 0.0f, 1.0f);
	if (DynamicMaterial3) {
		DynamicMaterial3->SetScalarParameterValue(FName("param_color"), NormalizedStamina);
	}
	if (StageManager)
	{
		ActivateInput(false);
	}

	// HitRange 초기 크기값 저장  
	initscale_hitrange = HitRange->GetComponentScale();
}

void APlayerCharacter::SetPlayerScale(float Attack_Range,float Dash_Range) {
	player_attack_range = Attack_Range;
	Dash_Power = Dash_Range;
	FVector scaleratio = FVector(player_attack_range, player_attack_range, 0.25f);
	Attack_Round->SetRelativeScale3D(scaleratio);
	float scale_value = 8.8f / 12.0f;
	float NewSpriteSize = player_attack_range * scale_value;

	player_arrow_maxdistance = 220.0f * (player_attack_range / 12.0f);
	player_arrow_mindistance = 34.0f * (player_attack_range / 12.0f);

	if (CircleEffectComponent)
	{
		CircleEffectComponent->SetVariableFloat(FName("circle_scale"), NewSpriteSize);
	}
}

void APlayerCharacter::SetPlayerState(float PlayerScale,float AttackRange, float DashRange,float ArrowFX) {
	FVector CurrentScale = RootComponent->GetComponentScale();
	FVector NewScale = CurrentScale * PlayerScale;
	RootComponent->SetWorldScale3D(NewScale);
	
	SetPlayerScale(StageManager->GetPlayerRange()* AttackRange / PlayerScale, StageManager->GetPlayerDashRange()* DashRange);

}

void APlayerCharacter::ShowWidgetOnBlockedView() {

	if (!PlayerController) return;

	FVector PlayerLocation = GetActorLocation();
	FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	FVector Start = FVector(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z + 40.0f);
	FVector End = CameraLocation;

	//LOG_SGS_SCREEN(3.f, TEXT("End Vector: X=%f, Y=%f, Z=%f"), End.X, End.Y, End.Z);
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);  // 플레이어 자신 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
	//FColor DebugColor = bHit ? FColor::Green : FColor::Red;  // 충돌이 있으면 녹색, 없으면 빨간색으로 표시
	//DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 2.0f, 0, 0.5f);  // 1초 동안 라인 표시
	if (bHit) {
		//히트된 것이 적일경우 무시함
		AActor* HitActor = HitResult.GetActor();
		ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(HitActor);
		if (Enemy || HitActor->Tags.Contains(FName("IgnoreViewWidget")))
		{
			return;
		}

		// 충돌한 지점의 월드 좌표를 화면 좌표(Screen Space)로 변환
		FVector2D ScreenPosition;
		//bool bProjected = PlayerController->ProjectWorldLocationToScreen(HitResult.ImpactPoint, ScreenPosition);
		bool bProjected = PlayerController->ProjectWorldLocationToScreen(PlayerLocation, ScreenPosition);

		if (bProjected) {
			if (!BlockWidgetInstance) {
				// 위젯이 없으면 생성
				BlockWidgetInstance = CreateWidget<UUserWidget>(PlayerController, BlockWidgetClass);
				if (BlockWidgetInstance) {
					BlockWidgetInstance->AddToViewport();
				}
			}
			if (BlockWidgetInstance) {
				// 위젯 위치를 충돌 지점의 화면 좌표로 설정
				BlockWidgetInstance->SetPositionInViewport(ScreenPosition);
				BlockWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	else {
		// 충돌이 없으면 위젯 숨기기
		if (BlockWidgetInstance) {
			BlockWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


//스태미너 사용시 스태미너 감소
bool APlayerCharacter::UseStamina(float Amount)
{

	if (Player_Current_Stamina >= Amount) {
		Player_Current_Stamina -= Amount;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Not enough stamina : %f"), Player_Current_Stamina);
		return false;
	}
}

void APlayerCharacter::Take_HP(int count, AActor* DamageCauser)
{
	if (StageManager->GetStageIndex() == EStageIndex::AdjustInputOffset)
	{
		return;
	}
	if (bIsInvincibility)
	{
		return;
	}
	FString CauserName = DamageCauser->GetName();
	LOG_KHJ(Display, TEXT("%s dealt damage to player"), *CauserName);

	if (count > 0) {

		Player_Current_HP += count;
		Player_Current_HP = FMath::Min(Player_MAX_HP, Player_Current_HP);
		//증가 UI 이벤트
		FeedBack_Fadeout(count);
		Change_HP_UI(count);
		OnStateChanged.Broadcast(Player_Current_HP, Player_Current_Health);
		
		//보너스점수 입력시마다 증가하는 기본 생존점수 제거
		//StageManager->AddScore(RECOVER_SCORE);
		//StageManager->PlayAddScoreAnim(RECOVER_SCORE);
		StageManager->PlaySfx(ESfx::PlayerHealed);
		//이펙트 출력
		if (IsValid(HealEffectComponent)) {
			HealEffectOpacity = 1.0f;
			HealEffectComponent->Activate(true);
		}
		
	}
	else {

		if (Can_ChangeMaterial) {
			//감소 UI 이벤트
			FeedBack_Fadeout(count);
			Player_Current_HP += count;
			Event_OnHit();
			Change_HP_UI(count);
			OnStateChanged.Broadcast(Player_Current_HP, Player_Current_Health);

			if (IsValid(HealEffectComponent)) {
				HealEffectComponent->SetVariableFloat(FName("LifeTime"), beatpersecond);
				//이펙트 출력
				HitEffectComponent->Activate(true);
			}

			if (Player_Current_HP < 0 && CanGameOver) {
				//튜토리얼 예외 처리
				if (StageManager->GetStageIndex() == EStageIndex::StageTutorial)
				{
					Player_Current_HP = 0;
					return;
				}
				StageManager->GameOver(EGameOverType::GameFailed_PlayerDied);
				CanGameOver = false;
			}

		}
	}
}

void APlayerCharacter::Change_HP_UI(int count) {
	//hp 변화 델리게이트 호출
	int changer = (count > 0) ? 1 : -1; //count가 양수면 1 아니면 -1 
	OnHPChanged.Broadcast(Player_Current_HP, changer);
	//UE_LOG(LogTemp, Warning, TEXT("changer count %d"), changer);
}

//	AttackPerfect, AttackGreat, AttackGood, AttackNormal, AttackFail, DashSuccess, DashFail
void APlayerCharacter::AttackJudge(EJudgementResult judgenote) {
	AttackJudgement = judgenote;
	switch (judgenote) {
	case AttackNormal:
		//Miss
		Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Normal_Multiplier;
		AttackColor = FLinearColor(0.392f, 0.392f, 0.392f, 1.0f);
		break;
	case AttackPerfect:
		Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Perfect_Multiplier;
		SetPlayer_Health(12.5f);
		AttackColor = FLinearColor(0.784f, 0.196f, 0.0f, 1.0f);
		break;
	case AttackGreat:
		Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Great_Multiplier;
		SetPlayer_Health(6.25f);
		AttackColor = FLinearColor(0.588f, 0.588f, 0.0f, 0.588f);
		break;
	case AttackGood:
		Player_Attack_Damage = Player_Attack_Damage_Base * Player_Attack_Damage_Good_Multiplier;
		SetPlayer_Health(3.125f);
		AttackColor = FLinearColor(0.0f, 0.588f, 0.0f, 1.0f);
		//Good
		break;

	}

}

void APlayerCharacter::BPM1_HP(int QuaterBeatIndex) {
	if (Player_Current_Stamina < Player_MAX_Stamina) {
		Player_Current_Stamina += 1.5625f;
		float NormalizedStamina = FMath::Clamp(Player_Current_Stamina / Player_MAX_Stamina, 0.0f, 1.0f);
		if (DynamicMaterial3 && !bIsCapeRed) {
			if (NormalizedStamina == 1.0f) {
				DynamicMaterial3->SetScalarParameterValue(FName("DashFail"), 0.5);
				return;
			}
			DynamicMaterial3->SetScalarParameterValue(FName("DashFail"), 0);
			DynamicMaterial3->SetScalarParameterValue(FName("param_color"), NormalizedStamina);

		}
	}
}

void APlayerCharacter::OnDashFail_ChangeCape() {
	if (DynamicMaterial3) {
		//0.5f 기준으로 0.5 == 풀스태미너, 1 == 빨강  0 == 파랑
		DynamicMaterial3->SetScalarParameterValue(FName("DashFail"), 1);
		bIsCapeRed = true;
	}
	FTimerHandle CapeColorResetHandle;
	// 일정 시간 뒤 원래 색으로 복구
	GetWorldTimerManager().SetTimer(
		CapeColorResetHandle,
		this,
		&APlayerCharacter::Rest_ChanageCape,
		beatpersecond /2,  // 0.5beat뒤 리셋
		false
	);
}


void APlayerCharacter::Rest_ChanageCape() {
	if (DynamicMaterial3) {
		DynamicMaterial3->SetScalarParameterValue(FName("DashFail"), 0);
	}
	bIsCapeRed = false;
}

//hp 페이드 아웃 호출
void APlayerCharacter::FeedBack_Fadeout(int count) {
	FadeoutWidget->FadeOut(beatpersecond, count);
}

//공격 모션속도 결정
void APlayerCharacter::Set_Montage_Speed(UAnimMontage* Montage, float goalduration) {
	if (!Montage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 애니메이션 길이
		float OriginalDuration = Montage->GetPlayLength();

		goalduration = goalduration * 0.75f;
		// 원하는 실행시간에 맞춰 배속(PlayRate) 0.5비트
		float PlayRate = OriginalDuration / goalduration;

		// 몽타주 실행 (재생 속도 조정)
		AnimInstance->Montage_Play(Montage, PlayRate);
	}
}

//해당 Target에게 대미지를 주는 함수(대미지, 피해를 받을 액터, 공격 타입)
void APlayerCharacter::ApplyDamage(float Damage, AActor* Target, EAttackType AttackType)
{
	if (!Target) return;
	//Enemy가 대미지를 받지 않는 상태라면 종료
	ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(Target);
	if (!CheckCanTargetEnemy(Enemy, AttackType)) return;
	//공격 타입에 따라 대시 판정 혹은 공격 판정을 보냄
	EJudgementResult result = EJudgementResult::AttackNormal; //default 추가
	if (AttackType == EAttackType::Normal) result = AttackJudgement;
	else if(AttackType == EAttackType::Dash) result = dashjudge;

	// TakeDamagePlayer 호출 (대미지, 공격판정, 공격자 전달)
	Enemy->TakeDamagePlayer(Damage, result, this);
}

void APlayerCharacter::OnAttackCompleted() {
	hit_list.Empty();
}

void APlayerCharacter::OnSwordMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(OtherActor);  // 한 번만 캐스팅

	if (Enemy && Check_Hit_Enemy(Enemy))
	{
		if (!hit_list.Contains(Enemy)) {
			hit_list.Add(Enemy);
			if (AttackJudgement == EJudgementResult::AttackPerfect)
			{
				StageManager->AddClearStrikeCount(1);
				//StageManager->AddScore(PERFECT_ATTACK_SCORE);
				StageManager->PlaySfx(ESfx::AttackHitPerfect);
			}
			else if (AttackJudgement == EJudgementResult::AttackGreat)
			{
				StageManager->SetNowCombo(0);
				//StageManager->AddScore(GREAT_ATTACK_SCORE);
				StageManager->PlaySfx(ESfx::AttackHitGreat);
			}
			else if (AttackJudgement == EJudgementResult::AttackGood)
			{
				StageManager->SetNowCombo(0);
				//StageManager->AddScore(GOOD_ATTACK_SCORE);
				StageManager->PlaySfx(ESfx::AttackHitGood);
			}
			else
			{
				StageManager->SetNowCombo(0);
				StageManager->PlaySfx(ESfx::AttackHitNormal);
			}
			//Enemy->TakeDamage(Player_Attack_Damage);
			ApplyDamage(Player_Attack_Damage, OtherActor, EAttackType::Normal);
		}
	}
}



void APlayerCharacter::PlayerMesh_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{


	if (bIsDashing) {
		Can_ChangeMaterial = false;
		ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(OtherActor);
		if (Check_Hit_Enemy(Enemy) && Enemy) {
			if (!hit_list.Contains(Enemy)) {
				hit_list.Add(Enemy);
				ApplyDamage(Player_Attack_Damage, OtherActor, EAttackType::Dash);

				if (dashjudge == EJudgementResult::DashPerfect) {
					StageManager->AddClearStrikeCount(1);
					StageManager->PlaySfx(ESfx::AttackHitPerfect);
				}
				else if (dashjudge == EJudgementResult::DashSuccess) {
					StageManager->SetNowCombo(0);
					StageManager->PlaySfx(ESfx::AttackHitNormal);
				}
			}
		}
	}

	/*
	Start UMETA(DisplayName = "Start"),
	Forward UMETA(DisplayName = "Up"),
	Backward UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
	*/

}

void APlayerCharacter::OnObstacleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherComponent->ComponentHasTag(TEXT("Wall"))) {
		//1LOG_SGS_SCREEN(3.f, TEXT("벽 충돌"));
		ECamDirection Direction = CamActor->GetDirection();
		if ((Direction == ECamDirection::Forward && OtherComponent->ComponentHasTag(TEXT("S_Wall"))) ||
			(Direction == ECamDirection::Backward && OtherComponent->ComponentHasTag(TEXT("N_Wall"))) ||
			(Direction == ECamDirection::Left && OtherComponent->ComponentHasTag(TEXT("E_Wall"))) ||
			(Direction == ECamDirection::Right && OtherComponent->ComponentHasTag(TEXT("W_Wall")))) {
			overlap_wall = true;

			/*
			if (player_attach_wall) {
				Detach_wall();
				LOG_SGS_SCREEN(3.f, TEXT("다른벽접촉,기존벽어태치 해제"));
			}
			*/
			//LOG_SGS_SCREEN(3.f, TEXT("overlap_wall value: %s"), overlap_wall ? TEXT("true") : TEXT("false"));
			AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
			player_attach_wall = true;

			//LOG_SGS_SCREEN(3.f, TEXT("벽에 attach됨"));
			//FString ActorName = OtherActor->GetName();
			//LOG_SGS_SCREEN(3.f, TEXT("Attach된 Actor 이름: %s"), *ActorName);
			//LOG_SGS_SCREEN(3.f, TEXT("Current BeatIndex: %d"), stageManager->GetBeatIndex());
		}
	}

	// 장애물 충돌 체크
	if (OtherActor->ActorHasTag(TEXT("obstacle")) || OtherActor->ActorHasTag(TEXT("BeatUnit"))) {
		if (!OtherActor->ActorHasTag(TEXT("notDisappear"))) {
			overlap_obstacle = true;
			OverlappingComponent = OtherComponent;
			//LOG_SGS_SCREEN(3.f, TEXT("장애물 투명화실행"));
		}
	}
}


void APlayerCharacter::OnObstacleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherComp->ComponentHasTag(TEXT("Wall"))) {
		overlap_wall = false;
		//1LOG_SGS_SCREEN(3.f, TEXT("오버랩에서 벗어나서 detach"));
		Detach_wall();
	}
	if ((OtherActor->ActorHasTag(TEXT("obstacle")) || OtherActor->ActorHasTag(TEXT("BeatUnit")))) {
		if (!OtherActor->ActorHasTag(TEXT("notDisappear"))) {
			overlap_obstacle = false;
			OverlappingComponent = nullptr;
		}
	}
}

void APlayerCharacter::Player_Attack_Round_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(OtherActor);
	if (Enemy && Check_Hit_Enemy(Enemy)) {

		if (OtherComponent && OtherComponent->ComponentHasTag(FName("NonTargetBoss"))) {
			return;
		}

		if (!OverlappingEnemies.Contains(Enemy)) {
			OverlappingEnemies.Add(Enemy);
		}
	}
}

void  APlayerCharacter::Refresh_enemy_area_Enemies()
{
	for (ABeatUnitCharacter_Enemy* Enemy : OverlappingEnemies)
	{
		if (!enemy_area.Contains(Enemy))
		{
			//일반 공격할 수 없는 적은 타겟팅 되지않는다
			if (!CheckCanTargetEnemy(Enemy, EAttackType::Normal)) continue;

			enemy_area.Add(Enemy); //조건 만족시 추가
		}
	}
}

//여기에 세가지 종류의 예외를 넣었음
// 일반 공격 피해 안 받을 경우
// 플레이어 시야에 들어오지 않았을 경우
// 보스: 특정 비트 인덱스 동안만 공격 가능한 경우
bool APlayerCharacter::CheckCanTargetEnemy(ABeatUnitCharacter_Enemy* Enemy, EAttackType attack)
{
	// 때릴수없으면 false, 때릴수있으면 true반환
	if (Enemy->bIsCommonMonster()) {
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(Enemy);
		if (!EnemyBase->IsInView()) return false;
		if (!EnemyBase->IsAvailableForAttack(attack)) return false;
	}
	else if (Enemy->bIsBossMonster()) {
		ABeatUnitCharacter_Boss_Harmyth* Boss = Cast<ABeatUnitCharacter_Boss_Harmyth>(Enemy);
		return Boss->bIsHitTime();
	}

	return true;
}

void APlayerCharacter::Player_Attack_Round_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	ABeatUnitCharacter_Enemy* Enemy = Cast<ABeatUnitCharacter_Enemy>(OtherActor);
	if (Enemy) {
		if (OtherComp && OtherComp->ComponentHasTag(FName("NonTargetBoss")))
		{
			return;
		}
		OverlappingEnemies.Remove(Enemy);
		enemy_area.Remove(Enemy);
	}
}

bool APlayerCharacter::IsObstacleInFront()
{
	if (!CamActor) return false;

	// 플레이어 위치,카메라 방향 가져오기
	FVector PlayerLocation = GetActorLocation();
	FVector ForwardVector;

	// 카메라 방향에 따른 벡터 설정
	ECamDirection Direction = CamActor->GetDirection();
	switch (Direction)
	{
	case ECamDirection::Forward:
		ForwardVector = FVector::ForwardVector; // +X 방향
		break;
	case ECamDirection::Backward:
		ForwardVector = FVector::BackwardVector; // -X 방향
		break;
	case ECamDirection::Left:
		ForwardVector = FVector::LeftVector; // -Y 방향
		break;
	case ECamDirection::Right:
		ForwardVector = FVector::RightVector; // +Y 방향
		break;
	default:
		return false;
	}

	// 라인 트레이스 지점(50cm까지)
	FVector Start = PlayerLocation;
	FVector End = Start + (ForwardVector * 50.0f);

	// 충돌 정보 저장 변수
	FHitResult HitResultCenter, HitResultLeft, HitResultRight;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 플레이어 무시

	// 플레이어 메쉬의 끝지점에서 라인 트레이스
	float PlayerMeshExtent = GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector Offset = FVector(0, PlayerMeshExtent, 0);
	FVector LeftStart = Start - Offset;
	FVector RightStart = Start + Offset;
	FVector LeftEnd = LeftStart + (ForwardVector * 50.0f);
	FVector RightEnd = RightStart + (ForwardVector * 50.0f);

	// 방향별 라인트레이스 실행(중앙은 필요없음)
	bool bLeftHit = GetWorld()->LineTraceSingleByChannel(HitResultLeft, LeftStart, LeftEnd, ECC_Visibility, QueryParams);
	bool bRightHit = GetWorld()->LineTraceSingleByChannel(HitResultRight, RightStart, RightEnd, ECC_Visibility, QueryParams);
	bool bCenterHit = GetWorld()->LineTraceSingleByChannel(HitResultCenter, Start, End, ECC_Visibility, QueryParams);

	DrawDebugLine(GetWorld(), LeftStart, LeftEnd, bLeftHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 2.0f);
	DrawDebugLine(GetWorld(), RightStart, RightEnd, bRightHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 2.0f);
	DrawDebugLine(GetWorld(), Start, End, bCenterHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 2.0f);

	// 장애물이 감지여부
	if (((bLeftHit && !HitResultLeft.GetActor()->ActorHasTag("Wall")) && (HitResultLeft.GetActor()->ActorHasTag(TEXT("obstacle")) || HitResultLeft.GetActor()->ActorHasTag(TEXT("BeatUnit")))) ||
		((bRightHit &&!HitResultRight.GetActor()->ActorHasTag("Wall")) && (HitResultRight.GetActor()->ActorHasTag(TEXT("obstacle")) || HitResultRight.GetActor()->ActorHasTag(TEXT("BeatUnit")))) ||
		((bCenterHit && !HitResultCenter.GetActor()->ActorHasTag("Wall")) && (HitResultCenter.GetActor()->ActorHasTag(TEXT("obstacle")) || HitResultCenter.GetActor()->ActorHasTag(TEXT("BeatUnit")))))
	{
		return true; // 장애물 있음
	}

	return false; // 장애물 없음
}
void APlayerCharacter::Obstacle_SetMaterial(UPrimitiveComponent* Component, UMaterialInterface* NewMaterial)
{
	if (!Component || !NewMaterial) return;  // Component나 NewMaterial이 nullptr인 경우, 함수를 종료

	int32 MaterialCount = Component->GetNumMaterials();  // 컴포넌트의 마테리얼 슬롯 수

	//지정된 마테리얼 인덱스를 순회하며 null일때까지 반복
	for (int32 Index = 0; Index < MaterialCount; ++Index)
	{

		if (Component->GetMaterial(Index) != nullptr)
		{

			Component->SetMaterial(Index, NewMaterial);
		}
		else
		{

			break;
		}
	}
}


//리스트를 통한 중복 적 확인(중복 공격 방지)
bool APlayerCharacter::Check_Hit_Enemy(ABeatUnitCharacter_Enemy* Enemy) {
	if (Enemy) {
		if (EnemyArray.Contains(Enemy)) {
			UE_LOG(LogTemp, Log, TEXT("Overlap with enemy"));
			return true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Overlap not enemy"));
		}
	}
	return false;
}

void APlayerCharacter::Event_OnHit()
{

	// 오파시티 값을 낮춤 (반투명)
	DynamicMaterial0->SetScalarParameterValue(FName("Parameter_Opacity"), 0.1f);
	DynamicMaterial1_2->SetScalarParameterValue(FName("Parameter_Opacity"), 0.1f);
	DynamicMaterial3->SetScalarParameterValue(FName("Parameter_Opacity"), 0.1f);

	StageManager->PlaySfx(ESfx::PlayerDamaged);
	Can_ChangeMaterial = false;
	// 일정 시간 후 다시 불투명하게 복구
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::ResetOpacity, (beatpersecond * 2.0f), false);
}




//원래 색으로 복구
void APlayerCharacter::ResetOpacity()
{
	DynamicMaterial0->SetScalarParameterValue(FName("Parameter_Opacity"), 1.0f);
	DynamicMaterial1_2->SetScalarParameterValue(FName("Parameter_Opacity"), 1.0f);
	DynamicMaterial3->SetScalarParameterValue(FName("Parameter_Opacity"), 1.0f);
	Can_ChangeMaterial = true;

}
//회복게이지 설정 
void APlayerCharacter::SetPlayer_Health(float health)
{
	Player_Current_Health += health;
	if (Player_Current_Health >= 100.0f) {
		Player_Current_Health -= 100.0f;
		Take_HP(1, this);
		if (StageManager->IsTutorialStage())
		{
			if (TutorialManager) {
				TutorialManager->OnTutorialHealthUp();
			}
		}
	}
	OnStateChanged.Broadcast(Player_Current_HP, Player_Current_Health);

}

void APlayerCharacter::AttachToCamera()
{
	AttachToActor(StageManager->GetCamera(), FAttachmentTransformRules::KeepWorldTransform);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
	HitRange->SetCollisionProfileName(TEXT("NoCollision"));
}

void APlayerCharacter::DetachFromCamera()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	HitRange->SetCollisionProfileName(TEXT("HitRange"));
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void APlayerCharacter::ActivateInput(bool bActivation)
{
	this->bIgnoreInput = !bActivation;
}

void APlayerCharacter::Detach_wall() {
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	player_attach_wall = false;
}

void APlayerCharacter::PauseGame()
{
	StageManager->SetStagePaused(true);
	StageManager->ShowPauseUI();
}

void APlayerCharacter::SetCanDash(bool bCan)
{
	bCanDash = bCan;
}