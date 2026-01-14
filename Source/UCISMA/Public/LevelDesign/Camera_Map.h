#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Map_Array_Actor.h"
#include "Camera_Map.generated.h"


class UStaticMeshComponent;
class APlayerCharacter;

USTRUCT()
struct FUnitData
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Unit;

	UPROPERTY()
	int32 BeatIndex;

	UPROPERTY()
	int32 QuaterIndex;


	FUnitData() : Unit(nullptr), BeatIndex(-1), QuaterIndex(-1){}
	FUnitData(TWeakObjectPtr<AActor> InUnit, int32 InBeatIndex, int32 InQuaterIndex) : Unit(InUnit), BeatIndex(InBeatIndex), QuaterIndex(InQuaterIndex) {}
};

UCLASS()
class UCISMA_API ACamera_Map : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ACamera_Map();
	virtual void Tick(float DeltaTime) override;

	void FindMap();

	void Move_Map(float DeltaTime);

	ECamDirection GetDirection();
	
	UFUNCTION(BlueprintCallable)
	void BeatUpdate(int QuaterIndex);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Setup_Player_Location();
	//해당 비트 인덱스가 몇 타일 인덱스이고 LerpAlpha는 얼마나 되는지 알려주는 함수이다. LerpAlpha값이 필요 없다면 BeatIndex만 인자로 보내면 된다.
	int BeatIndexToTileIndex(int BeatIndex, float* OutLerpAlpha = nullptr);
	//해당 타일이 현재 타일이 되는 순간의 BeatIndex를 반환하는 함수이다. -1이 반환되면 스킵 등으로 사용되지 않는 타일이다.
	int TileIndexToBeatIndex(int TileIndex);
	//각 타일의 시작 비트를 계산해두는 함수
	void InitTileStartBeatIndex();
	//타일의 초기설정을 가져오는 함수
	void InitTile();




	/*********************변수**********************/


	//오브젝트
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FloorPlane;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* E_Wall;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* W_Wall;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* S_Wall;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* N_Wall;


	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties")
	float LerpAlpha = 0.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> Tile_Position; // 타일의 위치들

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties", meta = (AllowPrivateAccess = "true"))
	TArray<ECamDirection> Tile_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties")

	int32 Before_Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties")

	int32 Current_Index = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties")
	TArray<FTileSkipInfo> Tile_Skip_Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties")
	TArray<float> Tile_Speed_Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Properties|[실행할beat,이동할 타일]")

	bool Start_Move = false;
	bool bIsCameraInit = false;
	
	void RegisterUnit(AActor* Unit, int32 BeatIndex, int32 QuaterIndex);
	void UnregisterUnit();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	void Find_Stage();
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//void FindMap_Create(); //Create_Map 맵핑시키는 함수

	bool IsValidTileIndex(int32 Index) const;

	void BeatSkipIndex(int tile_index); //비트이동함수



	AActor* Player;
	class APlayerCharacter* PlayerCharacter;
	int CamSpeed = 1;

	float BPMfloat = 0.0f;
	UPROPERTY(VisibleAnywhere)
	int CurrentBeatIndex = 0; //비트 횟수
	class AStageManager* StageManager;
	class AMap_Array_Actor* MapArrayManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PlayerFallingZone;

	TMap<int32, int32> Beat_Skip_Index; // 실행할 beat -> 이동할 타일 인덱스
	//각 타일의 시작 비트를 담은 인덱스
	UPROPERTY(visibleAnywhere)
	TArray<int> TileStartBeatIndex;
	//타일의 시작 인덱스를 초기화 했는지 판단하는 변수
	bool bIsInitTile;
	bool bIsSkipping;



	UPROPERTY()
	TArray<FUnitData> SpawnedEnemyUnits;



};