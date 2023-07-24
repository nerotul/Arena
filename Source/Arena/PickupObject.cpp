// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupObject.h"
#include "Components/SphereComponent.h"
#include "ArenaCharacter.h"
#include "InventoryComponent.h"
#include "HealthComponent.h"

// Sets default values
APickupObject::APickupObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(CollisionComponent);

}

// Called when the game starts or when spawned
void APickupObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupObject::Interact(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		AArenaCharacter* OverlappedCharacter = Cast<AArenaCharacter>(Interactor);

		if (IsValid(OverlappedCharacter))
		{
			switch (PickupType)
			{
				case PickupType::Ammo:
				{
					UInventoryComponent* CharacterInventory = OverlappedCharacter->CharacterInventory;

					if (CharacterInventory->GetInventoryAmmo(WeaponType) < CharacterInventory->GetMaxInventoryAmmo(WeaponType))
					{
						CharacterInventory->AddInventoryAmmo(WeaponType, StoredAmmo);
						SetActorHiddenInGame(true);
						SetActorEnableCollision(false);
						GetWorldTimerManager().SetTimer(ToggleVisibilityHandle, this, &APickupObject::ToggleVisibility, ObjectRespawnDelay, false);
					}

				}
					break;
				case PickupType::Armor:
				{
					UHealthComponent* CharacterHealth = OverlappedCharacter->CharacterHealth;
					if (CharacterHealth->GetCurrentArmor() < CharacterHealth->GetMaxArmor())
					{
						CharacterHealth->ChangeArmorValue(StoredHealthOrArmor);
						SetActorHiddenInGame(true);
						SetActorEnableCollision(false);
						GetWorldTimerManager().SetTimer(ToggleVisibilityHandle, this, &APickupObject::ToggleVisibility, ObjectRespawnDelay, false);
					}
				}
					break;
				case PickupType::Health:
				{
					UHealthComponent* CharacterHealth = OverlappedCharacter->CharacterHealth;
					if (CharacterHealth->GetCurrentHealth() < CharacterHealth->GetMaxHealth())
					{
						OverlappedCharacter->CharacterHealth->ChangeHealthValue(StoredHealthOrArmor);
						SetActorHiddenInGame(true);
						SetActorEnableCollision(false);
						GetWorldTimerManager().SetTimer(ToggleVisibilityHandle, this, &APickupObject::ToggleVisibility, ObjectRespawnDelay, false);
					}
				}
					break;
				default:
					break;
			}
		}
	}
}

void APickupObject::ToggleVisibility()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

}
