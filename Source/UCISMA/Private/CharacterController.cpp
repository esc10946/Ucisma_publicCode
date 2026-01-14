// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"

ACharacterController::ACharacterController()
{
}



void ACharacterController::BeginPlay()
{
    Super::BeginPlay(); 
    bShowMouseCursor = true;
}

void ACharacterController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);  
}