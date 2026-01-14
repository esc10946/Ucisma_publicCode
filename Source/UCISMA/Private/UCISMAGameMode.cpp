// Copyright Epic Games, Inc. All Rights Reserved.

#include "UCISMAGameMode.h"
#include "UCISMACharacter.h"
#include "UObject/ConstructorHelpers.h"

AUCISMAGameMode::AUCISMAGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdParty/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
