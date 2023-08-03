// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::SetInventoryAmmo(WeaponType InWeaponType, int InAmmoChange)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		InventoryRifleAmmo -= InAmmoChange;
		break;
	case WeaponType::Sniper:
		InventorySniperAmmo -= InAmmoChange;
		break;
	default:
		break;
	}
}

int UInventoryComponent::GetInventoryAmmo(WeaponType InWeaponType)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		return InventoryRifleAmmo;
		break;
	case WeaponType::Sniper:
		return InventorySniperAmmo;
		break;
	default:
		return 0;
		break;
	}
}

int UInventoryComponent::GetMaxInventoryAmmo(WeaponType InWeaponType)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		return MaxInventoryRifleAmmo;
		break;
	case WeaponType::Sniper:
		return MaxInventorySniperAmmo;
		break;
	default:
		return 0;
		break;
	}

}

void UInventoryComponent::SetInventoryMagazineAmmo(WeaponType InWeaponType, int InAmmo)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		InventoryRifleMagazineAmmo = InAmmo;
		break;
	case WeaponType::Sniper:
		InventorySniperMagazineAmmo = InAmmo;
		break;
	default:
		break;
	}
}

int UInventoryComponent::GetInventoryMagazineAmmo(WeaponType InWeaponType)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		return InventoryRifleMagazineAmmo;
		break;
	case WeaponType::Sniper:
		return InventorySniperMagazineAmmo;
		break;
	default:
		return 0;
		break;
	}
}

void UInventoryComponent::AddInventoryAmmo(WeaponType InWeaponType, int InAmmoChange)
{
	switch (InWeaponType)
	{
	case WeaponType::AR:
		if ((InventoryRifleAmmo + InAmmoChange) > MaxInventoryRifleAmmo)
		{
			InventoryRifleAmmo = MaxInventoryRifleAmmo;
		}
		else
		{
			InventoryRifleAmmo += InAmmoChange;
		}
		break;
	case WeaponType::Sniper:
		if ((InventorySniperAmmo + InAmmoChange) > MaxInventorySniperAmmo)
		{
			InventorySniperAmmo = MaxInventorySniperAmmo;
		}
		else
		{
			InventorySniperAmmo += InAmmoChange;
		}
		break;
	default:
		break;
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryRifleAmmo);
	DOREPLIFETIME(UInventoryComponent, InventoryRifleMagazineAmmo);
	DOREPLIFETIME(UInventoryComponent, InventorySniperAmmo);
	DOREPLIFETIME(UInventoryComponent, InventorySniperMagazineAmmo);

}