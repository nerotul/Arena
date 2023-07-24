// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ArenaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ARENA_API AArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int PlayerScore = 0;

public:
	UFUNCTION(BlueprintCallable)
		int GetPlayerScore();

	UFUNCTION()
		void IncrementPlayerScore();

	UFUNCTION()
		void DecrementPlayerScore();
};
