#include "LevelDesign/Camera_Map.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "BeatUnit/BeatUnitInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Manager/UIManager.h"
#include "BeatUnit/Enemy/EnemyBase.h"
#include "Widget/Widget_Purification.h"
#include "PlayerCharacter.h"


ACamera_Map::ACamera_Map()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	// 바닥 플레인 생성
	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	FloorPlane->SetupAttachment(RootComponent);

	// 벽 4개 생성
	N_Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("N_Wall"));
	N_Wall->SetupAttachment(RootComponent);

	S_Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("S_Wall"));
	S_Wall->SetupAttachment(RootComponent);

	E_Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("E_Wall"));
	E_Wall->SetupAttachment(RootComponent);

	W_Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("W_Wall"));
	W_Wall->SetupAttachment(RootComponent);


	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraZone"));
	BoxCollision->SetupAttachment(RootComponent);
	if (!BoxCollision)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] Camera_Map > Contruct > BoxCollision is nullptr!"));
	}
	PlayerFallingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("Player Failling Zone"));
	PlayerFallingZone->SetupAttachment(RootComponent);
	if (!PlayerFallingZone)
	{
		LOG_DEBUG(Error, TEXT("Player Falling Zone is nullptr!"));
	}
	// 카메라 컴포넌트 생성
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	bIsSkipping = false;
}

// Called when the game starts or when spawned
void ACamera_Map::BeginPlay()
{
	Super::BeginPlay();
	
	InitTile();

	if (!BoxCollision)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] Camera_Map > BeginPlay > BoxCollision is nullptr!"));
		return;
	}

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACamera_Map::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACamera_Map::OnOverlapEnd);
	PlayerFallingZone->OnComponentBeginOverlap.AddDynamic(this, &ACamera_Map::OnOverlapBegin);


	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), FoundActors);

	if (FoundActors.Num() > 0)
	{
		Player = FoundActors[0];
		if (Player) {
			PlayerCharacter = Cast<APlayerCharacter>(Player);
		}
	}
	 


	
	
}

void ACamera_Map::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (Start_Move) {
		Move_Map(DeltaTime);
		if (!bIsCameraInit)
		{
			Setup_Player_Location();
			bIsCameraInit = true;
		}
	}
	/*
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("%f"), LerpAlpha));
	UE_LOG(LogTemp, Warning, TEXT("-----current %d ---"), Current_Index);
	UE_LOG(LogTemp, Warning, TEXT("-----before %d ---"), Before_Index);
	*/


}

void ACamera_Map::InitTile()
{
	if (bIsInitTile)
	{
		return;
	}
	bIsInitTile = true;

	Find_Stage();
	//FindMap_Create();
	FindMap();
	InitTileStartBeatIndex();
}


//패키징 시 해당 함수로 변경

void ACamera_Map::FindMap()
{
	// "Tag_Map" 태그가 붙은 모든 액터 검색
	TArray<AActor*> MapActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Tag_Map"), MapActors);

	// 검색된 액터가 존재하는지 확인
	if (MapActors.Num() > 0)
	{
		// 첫 번째 액터를 ACreateMap 클래스로 캐스팅
		MapArrayManager = Cast<AMap_Array_Actor>(MapActors[0]);
		if (MapArrayManager)
		{
			// 변수 가져오기
			Tile_Position = MapArrayManager->Tile_Position;
			Tile_Direction = MapArrayManager->Tile_Direction;
			Tile_Skip_Index = MapArrayManager->Tile_Skip_Index;
			Tile_Speed_Index = MapArrayManager->Tile_Speed_Index;
			Beat_Skip_Index = MapArrayManager->Beat_Skip_Index;


		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("캐스팅 실패: Tag_Map 액터가 ACreateMap 클래스가 아닙니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tag_Map 태그를 가진 액터를 찾을 수 없습니다."));
	}
}





bool ACamera_Map::IsValidTileIndex(int32 Index) const
{
	return Index >= 0 && Index < Tile_Position.Num();
}


void ACamera_Map::Move_Map(float DeltaTime)
{
	//타일이 1개 이하일 경우 종료
	if (Tile_Position.Num() <= 1)
	{
		Start_Move = false;
		return;
	}

	//현재 타일의 인덱스가 마지막 인덱스를 넘어갈 경우 종료
	if (Current_Index >= Tile_Skip_Index.Num() || Current_Index >= Tile_Speed_Index.Num())
	{
		Start_Move = false;
		return;
	}

	const FTileSkipInfo& TileInfo = Tile_Skip_Index[Current_Index];

	// ===================== 일반 이동 (Lerp) =====================
	if (!TileInfo.bSkipTile)
	{
		//float BPMfloat = static_cast<float>(StageManager->GetBpm()); //stagemanager 호출시 선언함 (bpm 고정상태일때만)
		float MoveSpeed = (BPMfloat / 240.0f) * DeltaTime * CamSpeed * Tile_Speed_Index[Current_Index];
		LerpAlpha += MoveSpeed;

		// 여러 타일을 건너뛸 수 있으므로 while 사용
		while (LerpAlpha >= 1.0f)
		{
			LerpAlpha -= 1.0f;
			Before_Index = Current_Index;

			if (++Current_Index >= Tile_Position.Num())
			{
				Start_Move = false;
				return;
			}
		}

		if (!IsValidTileIndex(Before_Index) || !IsValidTileIndex(Current_Index))
			return;

		FVector InterpolatedLocation = FMath::Lerp(Tile_Position[Before_Index], Tile_Position[Current_Index], LerpAlpha);
		SetActorLocation(InterpolatedLocation);
	}

	// ===================== 스킵타일 로직 (Skip) =====================
	else
	{
		bIsSkipping = true;
		if (++Current_Index >= Tile_Position.Num())
		{
			Start_Move = false;
			return;
		}
		//활성화된 적 삭제
		UnregisterUnit();
		if (!IsValidTileIndex(Before_Index) || !IsValidTileIndex(Current_Index))
			return;

		FVector Delta = Tile_Position[Current_Index] - Tile_Position[Before_Index];

		if (Player)
		{
			FVector PlayerLocation = Player->GetActorLocation();
			FVector NewPlayerLocation;

			if (PlayerCharacter) {
				PlayerCharacter->Detach_wall();
			}

			//플레이어 위치 상속여부(true일경우 위치 조정. false일 경우 그대로)
			if (TileInfo.bOverrideLocation)
			{
				if (TileInfo.OverrideLocation.IsNearlyZero())
				{
					NewPlayerLocation = GetActorLocation() + Delta;
				}
				else {
					NewPlayerLocation = GetActorLocation() + Delta + TileInfo.OverrideLocation;
				}
			}
			else {
				NewPlayerLocation = FVector(PlayerLocation.X + Delta.X, PlayerLocation.Y + Delta.Y, PlayerLocation.Z);
			}
			NewPlayerLocation.Z = PlayerLocation.Z; // Z축 위치는 유지
			Player->SetActorLocation(NewPlayerLocation);

		}

		SetActorLocation(GetActorLocation() + Delta);
		Before_Index = Current_Index;

		// 추가 타일 존재 시 다음 인덱스로
		if (Current_Index + 1 < Tile_Position.Num())
		{
			Current_Index++;
		}
		else
		{
			Start_Move = false;
		}
		bIsSkipping = false;
	}
}

void ACamera_Map::BeatSkipIndex(int tile_index) {
	bIsSkipping = true;
	UnregisterUnit();
	Start_Move = false;
	FVector CurrentLocation = GetActorLocation();
	FVector Delta = Tile_Position[tile_index] - CurrentLocation;

	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		Player->SetActorLocation(FVector(PlayerLocation.X + Delta.X, PlayerLocation.Y + Delta.Y, PlayerLocation.Z));
	}
	SetActorLocation(Tile_Position[tile_index]);
	Before_Index = tile_index;
	Current_Index = tile_index+1;
	LerpAlpha = 0.0f;
	Start_Move = true;
	bIsSkipping = false;
	
}

void ACamera_Map::Find_Stage() {
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
			StageManager->BeatUpdateListeners.AddDynamic(this, &ACamera_Map::BeatUpdate);
			CamSpeed = StageManager->GetCameraSpeed();
			BPMfloat = static_cast<float>(StageManager->GetBpm()); //bpm
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No StageManager found in the level."));
	}
}


ECamDirection ACamera_Map::GetDirection() {
	if (Tile_Direction.Num() > 1 && Current_Index < Tile_Direction.Num()) {
		/*
		FString DirectionName = StaticEnum<ECamDirection>()->GetNameStringByValue((int64)Tile_Direction[Current_Index]);
		LOG_SGS_SCREEN(3.f, TEXT("Direction: %s"), *DirectionName);
		*/
		return Tile_Direction[Current_Index];
	}
	return ECamDirection::Start;
}
void ACamera_Map::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(OtherActor);
	if (OverlappedComponent == BoxCollision && EnemyBase && !EnemyBase->IsInView())
	{
		EnemyBase->SetbInView(true);
		return;
	}

	APlayerCharacter* OverlapPlayer = Cast<APlayerCharacter>(OtherActor);
	if (OverlappedComponent == PlayerFallingZone && OverlapPlayer)
	{
		OverlapPlayer->SetActorLocation(FVector(OverlapPlayer->GetActorLocation().X, OverlapPlayer->GetActorLocation().Y, this->GetActorLocation().Z + 5));
		return;
	}
}
/*
DEFINE_FUNCTION(ACamera_Map::execOnOverlapEnd)
{
	P_GET_OBJECT(UPrimitiveComponent, Z_Param_OverlappedComp);
	P_GET_OBJECT(AActor, Z_Param_OtherActor);
	P_GET_OBJECT(UPrimitiveComponent, Z_Param_OtherComp);
	P_GET_PROPERTY(FIntProperty, Z_Param_OtherBodyIndex);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnOverlapEnd(Z_Param_OverlappedComp, Z_Param_OtherActor, Z_Param_OtherComp, Z_Param_OtherBodyIndex);
	P_NATIVE_END;
}
*/
void ACamera_Map::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IBeatUnitInterface* OtherBeatUnitInterface = Cast<IBeatUnitInterface>(OtherActor);
	if (OtherBeatUnitInterface)
	{
		//만약 이미 비활성화 된 액터가 화면에서 벗어난 것이라면 (즉, 플레이어가 죽인 경우)
		if (!OtherBeatUnitInterface->GetIsLiving())
		{
			return;
		}

		//화면에서 벗어난 액터가 정화 관련 액터인지 확인
		if (StageManager && OtherActor->ActorHasTag(TEXT("PurificationTarget")))
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (!PlayerController || !PlayerController->PlayerCameraManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Error] Camera_Map > OnOverlapEnd > Can't call PlayerController"));
			}
			FVector2D ActorScreenPos;
			PlayerController->ProjectWorldLocationToScreen(OtherActor->GetActorLocation(), ActorScreenPos);
			StageManager->FailPurify(ActorScreenPos);
			//StageManager->SetStagePaused(true);
			OtherBeatUnitInterface->DeactiveUnit();
		}
	}
	APlayerCharacter* OverlapPlayer = Cast<APlayerCharacter>(OtherActor);
	if (OverlapPlayer && !bIsSkipping && OverlapPlayer->Player_Current_HP > -1)
	{
		Setup_Player_Location();
		if (StageManager) {
			StageManager->PrintPlayerOutMessage();
		}
	}
}

void ACamera_Map::BeatUpdate(int QuaterIndex)
{
	Start_Move = true;

	if (QuaterIndex) return;

	if (Beat_Skip_Index.Contains(StageManager->GetBeatIndex()))
	{
		int32 TileIndex = Beat_Skip_Index[StageManager->GetBeatIndex()];
		BeatSkipIndex(TileIndex);
	}


	/*
	//LOG_SGS_SCREEN(3.f, TEXT("비트업데이트 실행"));
	if (StageManager) {
		StageManager->BeatUpdateListeners.RemoveDynamic(this, &ACamera_Map::BeatUpdate);
		//LOG_SGS_SCREEN(3.f, TEXT("BeatUpdate 제거 시도"));
	}

	*/

}
void ACamera_Map::Setup_Player_Location()
{

	if (!Player)	return;
    FVector FloorLocation = FloorPlane->GetComponentLocation();
    FVector FloorExtent = FloorPlane->GetStaticMesh()->GetBounds().BoxExtent;

    FVector MinBounds = FloorLocation - FloorExtent;
    FVector MaxBounds = FloorLocation + FloorExtent;

    FVector PlayerLocation = Player->GetActorLocation();

    if (PlayerLocation.X < MinBounds.X || PlayerLocation.X > MaxBounds.X ||
        PlayerLocation.Y < MinBounds.Y || PlayerLocation.Y > MaxBounds.Y)
    {
        FVector NewPlayerLocation = FloorLocation;
		NewPlayerLocation.Z = this->GetActorLocation().Z + 5;
        Player->SetActorLocation(NewPlayerLocation);
    }
}

int ACamera_Map::BeatIndexToTileIndex(int BeatIndex, float* OutLerpAlpha)
{
	int RemainingBeat = BeatIndex;
	int BeatOfTile = 4 / Tile_Speed_Index[0];
	int ReturnTileIndex = 0;

	while (RemainingBeat >= BeatOfTile)
	{
		RemainingBeat -= BeatOfTile;
		ReturnTileIndex++;
		BeatOfTile = 4 / Tile_Speed_Index[ReturnTileIndex];
	}
	if (OutLerpAlpha)
	{
		*OutLerpAlpha = static_cast<float>(RemainingBeat) / static_cast<float>(BeatOfTile);
	}

	return ReturnTileIndex;
}


int ACamera_Map::TileIndexToBeatIndex(int TileIndex)
{
	if (!bIsInitTile)
	{
		InitTile();
	}
	TileIndex = FMath::Max(TileIndex, 0);
	TileIndex = FMath::Min(TileIndex, TileStartBeatIndex.Num()-1);

	return TileStartBeatIndex[TileIndex];
}

void ACamera_Map::InitTileStartBeatIndex()
{

	int LastBeatIndex = StageManager->GetLastBeatIndex();
	int CurrentTileIndex = 0;
	int NextTileIndex = 0;
	int NextTileBeat = 0;
	int LastTileIndex = Tile_Position.Num() - 1;
	
	TileStartBeatIndex.SetNum(LastTileIndex);
	for (int NowBeat = 0; NowBeat <= LastBeatIndex; NowBeat++)
	{
		if (NextTileIndex > LastTileIndex)
		{
			break;
		}
		//비트 스킵 타일의 경우 처리
		if (Beat_Skip_Index.Contains(NowBeat))
		{
			//스킵되는 구간까지 인덱스를 -1로 수정
			for (int TileIndex = NextTileIndex; TileIndex < Beat_Skip_Index[NowBeat]; TileIndex++)
			{
				TileStartBeatIndex[TileIndex] = -1;
			}

			NextTileBeat = NowBeat;
			TileStartBeatIndex[Beat_Skip_Index[NowBeat]] = NextTileBeat;
			NextTileBeat = NextTileBeat + 4 * 1 / Tile_Speed_Index[Beat_Skip_Index[NowBeat]+1];
			NextTileIndex = Beat_Skip_Index[NowBeat] + 1;

			continue;
		}

		if (NowBeat == NextTileBeat)
		{
			if (NextTileIndex >= LastTileIndex)
			{
				return;
			}
			TileStartBeatIndex[NextTileIndex] = NextTileBeat;
			CurrentTileIndex = NextTileIndex;
			NextTileIndex++;
			//스킵타일 고려
			if (Tile_Skip_Index[NextTileIndex].bSkipTile)
			{
				while (NextTileIndex <= LastTileIndex && Tile_Skip_Index[NextTileIndex].bSkipTile)
				{
					TileStartBeatIndex[NextTileIndex - 1] = -1;
					TileStartBeatIndex[NextTileIndex] = -1;
					CurrentTileIndex++;
					NextTileIndex++;
				}
				CurrentTileIndex++;
				NextTileIndex++;

				if (NextTileIndex >= LastTileIndex)
				{
					return;
				}

				TileStartBeatIndex[CurrentTileIndex] = NextTileBeat;

				
			}
			
			//다음 타일의 예상 시작 비트를 계산
			if (Tile_Speed_Index[NextTileIndex] > 0.01f)
			{
				NextTileBeat = NextTileBeat + 4 * 1 / Tile_Speed_Index[NextTileIndex];
			}
			else
			{
				NextTileBeat = -1;
			}
		}
	}
}


void ACamera_Map::RegisterUnit(AActor* Unit, int32 BeatIndex, int32 QuaterIndex) {
	if (Unit) {
		FUnitData NewUnitData(Unit, BeatIndex, QuaterIndex);
		SpawnedEnemyUnits.Add(NewUnitData);
	}
}

void ACamera_Map::UnregisterUnit()
{
	
	for (int32 i = SpawnedEnemyUnits.Num() - 1; i >= 0; --i)
	{
		FUnitData& UnitData = SpawnedEnemyUnits[i];

		if (!UnitData.Unit.IsValid())
		{
			SpawnedEnemyUnits.RemoveAt(i);
			continue;
		}

		if (UnitData.BeatIndex < StageManager->GetBeatIndex())
		{
			AEnemyBase* EnemyBase = Cast<AEnemyBase>(UnitData.Unit.Get());
			if (EnemyBase && EnemyBase->IsInView())
			{
				LOG_SGS_SCREEN(3.f, TEXT("삭제되는 유닛: %s"), *EnemyBase->GetName());
				EnemyBase->DeactiveUnit();
				SpawnedEnemyUnits.RemoveAt(i);
			}
		}

		else if (UnitData.BeatIndex == StageManager->GetBeatIndex()) {
			if (UnitData.QuaterIndex < StageManager->GetQuaterBeatIndex()) {
				AEnemyBase* EnemyBase = Cast<AEnemyBase>(UnitData.Unit.Get());
				if (EnemyBase && EnemyBase->IsInView())
				{
					LOG_SGS_SCREEN(3.f, TEXT("삭제되는 유닛: %s"), *EnemyBase->GetName());
					EnemyBase->DeactiveUnit();
					SpawnedEnemyUnits.RemoveAt(i);
				}
			}

		}
	}
}