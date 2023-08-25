// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

UCLASS(minimalapi)
class AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	void RespawnCharacter(AController* CharacterController, bool InIsNPC);

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> DefaultNPC = nullptr;

	UFUNCTION(BlueprintCallable)
	bool IsSpawnPointSafe(FVector SphereLocation);

	UPROPERTY(EditDefaultsOnly)
	float SpawnCheckRadius = 1000.0f;
};



