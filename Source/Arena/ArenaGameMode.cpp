// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaHUD.h"
#include "ArenaCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "ArenaPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"


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

void AArenaGameMode::RespawnCharacter(AController* CharacterController, bool InIsNPC)
{
	if (CharacterController)
	{
		if (HasAuthority())
		{
			if (!InIsNPC)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Respawning Character"));
				FVector Location = FVector(ChoosePlayerStart(CharacterController)->GetActorLocation());
				do
				{
					Location = FVector(ChoosePlayerStart(CharacterController)->GetActorLocation());

				} while (IsSpawnPointSafe(Location) == false);

				APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, FRotator::ZeroRotator);

				if (Pawn)
				{
					CharacterController->Possess(Pawn);
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Respawning Character"));
				FVector Location = FVector(ChoosePlayerStart(CharacterController)->GetActorLocation());
				if (IsSpawnPointSafe(Location) == true)
				{
					APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultNPC, Location, FRotator::ZeroRotator);

					if (Pawn)
					{
						CharacterController->Possess(Pawn);
					}

				}
			}
		}
	}
}

bool AArenaGameMode::IsSpawnPointSafe(FVector SphereLocation)
{
	TArray <TEnumAsByte<EObjectTypeQuery>> ObjectTypeArray;
	UClass* Class = AArenaCharacter::StaticClass();
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OverlappedActors;

	bool Result = !UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereLocation, SpawnCheckRadius, ObjectTypeArray, Class, ActorsToIgnore, OverlappedActors);
	UE_LOG(LogTemp, Warning, TEXT("Check result: %s"), (Result ? TEXT("true") : TEXT("false")));

	return Result;
}
