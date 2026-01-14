// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDesign/Map_Array_Actor.h"

// Sets default values
AMap_Array_Actor::AMap_Array_Actor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


}

// Called when the game starts or when spawned
void AMap_Array_Actor::BeginPlay()
{
	Super::BeginPlay();

}

void AMap_Array_Actor::Save_Tile() {
	Modify();
	MarkPackageDirty();
}