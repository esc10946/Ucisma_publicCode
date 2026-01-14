// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SoundManager.h"
#include "Manager/StageManager.h"
#include "Components/AudioComponent.h"
#include "LevelDesign/GameDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASoundManager::Init(AStageManager* InputStageManager)
{
	StageManager = InputStageManager;
	this->Song = InputStageManager->GetSong();
	this->SongLength = InputStageManager->GetSongLength();
	Sfx = StageManager->GetGameDataAsset()->GetSfx();
}

void ASoundManager::StartSong()
{
	if (!AudioComponent)
	{
		LOG_DEBUG(Warning, TEXT("AudioComponent Error"));
	}
	if (Song)
	{
		AudioComponent->SetSound(Song);
		AudioComponent->SetPitchMultiplier(StageManager->GetGameSpeed());
		AudioComponent->Play(SongStartTime);
	}
}

void ASoundManager::PauseSong()
{
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->SetPaused(true);
	}
}

void ASoundManager::ResumeSong()
{
	if (AudioComponent && AudioComponent->bIsPaused)
	{
		AudioComponent->SetPaused(false);
	}
}

void ASoundManager::PlaySfx(ESfx SfxType)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Sfx[SfxType]);
}

void ASoundManager::SetSongStartTime(float Value)
{
	SongStartTime = Value;
	SongStartTime = FMath::Max(0, Value);
}
