
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UCISMA/UCISMA.h"
#include "GameFramework/Actor.h"
#include "Enum_CamDirection.h"
#include "Map_Array_Actor.generated.h"


USTRUCT(BlueprintType)
struct FTileSkipInfo
{
	GENERATED_BODY()

	// 이 타일을 스킵할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Skip")
	bool bSkipTile = false;

	// 위치 설정 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Skip", meta = (EditCondition = "bSkipTile"))
	bool bOverrideLocation = false;

	// 이동할 위치 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Skip", meta = (EditCondition = "bSkipTile && bOverrideLocation"))
	FVector OverrideLocation = FVector::ZeroVector;
};


UCLASS()
class UCISMA_API AMap_Array_Actor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMap_Array_Actor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array")
	TArray<FVector> Tile_Position;

	// 각 위치에 해당하는 방향을 저장하는 enum 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array")
	TArray<ECamDirection> Tile_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array")
	int32 CurrentIndex;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array")
	TArray<FTileSkipInfo> Tile_Skip_Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array")
	TArray<float> Tile_Speed_Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Array|[실행할beat,이동할 타일]")
	TMap<int32, int32> Beat_Skip_Index; // 실행할 beat -> 이동할 타일 인덱스

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Save_Tile();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};