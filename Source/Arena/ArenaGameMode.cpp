// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaHUD.h"
#include "ArenaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AArenaGameMode::AArenaGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AArenaHUD::StaticClass();
}

void AArenaGameMode::RespawnCharacter(AController* CharacterController)
{
	if (CharacterController)
	{
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("IN RESPAWN"));
			FVector Location = FVector(-350, -100, 265);
			APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, FRotator::ZeroRotator);
			if (Pawn)
			{
				CharacterController->Possess(Pawn);
			}
		}
	}
}
