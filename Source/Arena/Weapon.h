// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UArrowComponent;
class USoundBase;
class UParticleSystem;
class AArenaCharacter;
class AArenaProjectile;
class AActor;


UCLASS()
class ARENA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* FPWeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* MuzzleLocation;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<AArenaProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* FireFX;

	void Fire(FRotator InSpawnRotation);
};