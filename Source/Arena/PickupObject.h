// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "ArenaInterface.h"
#include "PickupObject.generated.h"

class USphereComponent;
class UPointLightComponent;

UENUM(BlueprintType)
enum class PickupType : uint8
{
	Ammo UMETA(DisplayName = "Ammo"),
	Armor   UMETA(DisplayName = "Armor"),
	Health UMETA(DisplayName = "Health")
};


UCLASS()
class ARENA_API APickupObject : public AActor, public IArenaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		UPointLightComponent* PointLight = nullptr;

	UPROPERTY(EditAnywhere)
		PickupType PickupType;

	UPROPERTY(EditAnywhere)
		WeaponType WeaponType;

	UPROPERTY(EditAnywhere)
		int StoredAmmo = 30;

	UPROPERTY(EditAnywhere)
		float StoredHealthOrArmor = 30;


	virtual void Interact(AActor* Interactor) override;

	FTimerHandle ToggleVisibilityHandle;

	void ToggleVisibility();

	UPROPERTY(EditAnywhere)
		float ObjectRespawnDelay = 3.0f;

};
