// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaHUD.h"
#include "ArenaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "ArenaPlayerState.h"


AArenaGameMode::AArenaGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AArenaHUD::StaticClass();

	PlayerStateClass = AArenaPlayerState::StaticClass();
}

void AArenaGameMode::RespawnCharacter(AController* CharacterController)
{
	if (CharacterController)
	{
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Respawning Character"));
			FVector Location = FVector(ChoosePlayerStart(CharacterController)->GetActorLocation());
			APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, FRotator::ZeroRotator);
			if (Pawn)
			{
				CharacterController->Possess(Pawn);
				
			}
		}
	}
}
