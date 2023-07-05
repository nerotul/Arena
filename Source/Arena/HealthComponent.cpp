// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHealthComponent::GetCurrentArmor()
{
	return CurrentArmor;
}

void UHealthComponent::ChangeArmorValue(float ChangeValue)
{
	bool bIsDamage = (ChangeValue < 0);
	
	if(bIsDamage)
	{
		if ((GetCurrentArmor() + ChangeValue) < 0)
		{
			CurrentArmor += ChangeValue;
			ChangeHealthValue(CurrentArmor);
			CurrentArmor = 0.0f;
		}
		else
		{
			CurrentArmor += ChangeValue;
		}
	}
	else
	{
		(GetCurrentArmor() + ChangeValue) > MaxArmor ? CurrentArmor = MaxArmor : CurrentArmor += ChangeValue;
	}
	
}

float UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UHealthComponent::ChangeHealthValue(float ChangeValue)
{
	bool bIsDamage = (ChangeValue < 0);

	if (bIsDamage)
	{
		GetCurrentArmor() > 0 ? ChangeArmorValue(ChangeValue) : CurrentHealth += ChangeValue;

		if (CurrentHealth <= 0)
		{
			OnCharacterDead.Broadcast();
		}
	}
	else
	{
		(GetCurrentHealth() + ChangeValue) > MaxHealth ? CurrentHealth = MaxHealth : CurrentHealth += ChangeValue;
		
	}
	
}

