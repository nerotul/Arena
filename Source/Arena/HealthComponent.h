// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


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

	float MaxShield = 100;
	float CurrentShield = 100;
	float ShieldRecoveryRate = 1;

	float MaxHealth = 160;
	float CurrentHealth = 133;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetCurrentShield();
	void ChangeShieldValue(float ChangeValue);

	UFUNCTION(BlueprintCallable)
		float GetCurrentHealth();
	void ChangeHealthValue(float ChangeValue);

		
};
