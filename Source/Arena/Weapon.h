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
class ADropObject;


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= FX)
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		UParticleSystem* FXFire;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<AArenaProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<ADropObject> ShellClass;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UArrowComponent* ShellEjectDirection;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponChanged)
		AArenaCharacter* OwningCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<ADropObject> MagazineClass;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UArrowComponent* MagazineEjectDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		USoundBase* ReloadSound;



	void Fire(FRotator InSpawnRotation);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastOnFireFX();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxMagazineAmmo = 30;
	UPROPERTY(Replicated, BlueprintReadOnly)
		int CurrentMagazineAmmo = 30;

	void ReloadWeapon();

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastReloadFX();

	UPROPERTY(Replicated)
		bool bCanReload = true;
	UPROPERTY(Replicated)
		bool bCanFire = true;

	FTimerHandle ReloadTimerHandle;

	UFUNCTION(Server, Reliable)
	void ServerToggleReloadRestrictions();

	UPROPERTY(EditDefaultsOnly, Category = FireMechanics)
		bool bIsPhysicalProjectile = true;

	UFUNCTION()
		void OnRep_WeaponChanged();

};
