// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerState.h"
#include "Net/UnrealNetwork.h"

int AArenaPlayerState::GetPlayerScore()
{
	return PlayerScore;
}

void AArenaPlayerState::IncrementPlayerScore()
{
	PlayerScore++;
}

void AArenaPlayerState::DecrementPlayerScore()
{
	PlayerScore--;
}

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaPlayerState, PlayerScore);

}
