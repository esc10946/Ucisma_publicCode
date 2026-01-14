// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatUnit/BeatUnitActor_Fade.h"



ABeatUnitActor_Fade::ABeatUnitActor_Fade() {
}
void ABeatUnitActor_Fade::DeactiveUnit() {
    Super::DeactiveUnit();
}
void ABeatUnitActor_Fade::BeginPlay() {
    Super::BeginPlay();
}
void ABeatUnitActor_Fade::ActiveUnit()
{
    Super::ActiveUnit();
    SpawnWidget();
}