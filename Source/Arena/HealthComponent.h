// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float MaxArmor = 100.0f;
	float CurrentArmor = 100.0f;

	float MaxHealth = 100.0f;
	float CurrentHealth = 100.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		float GetCurrentArmor();
	void ChangeArmorValue(float ChangeValue);

	UFUNCTION(BlueprintCallable)
		float GetCurrentHealth();
	void ChangeHealthValue(float ChangeValue);

	FOnCharacterDead OnCharacterDead;
		
};