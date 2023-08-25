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


UENUM(BlueprintType)
enum class WeaponType : uint8
{
	AR UMETA(DisplayName = "AR"),
	Sniper   UMETA(DisplayName = "Sniper"),
};

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
		USkeletalMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		USkeletalMesh* TPWeaponMesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* MuzzleLocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= FX)
		USoundBase* FireSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		UParticleSystem* FXFire = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<AArenaProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<ADropObject> ShellClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UArrowComponent* ShellEjectDirection = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponChanged)
		AArenaCharacter* OwningCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		TSubclassOf<ADropObject> MagazineClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UArrowComponent* MagazineEjectDirection = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		USoundBase* ReloadSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponType)
		WeaponType WeaponType = WeaponType::AR;



	void Fire(FRotator InSpawnRotation);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastOnFireFX();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxMagazineAmmo = 30;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
		int CurrentMagazineAmmo = MaxMagazineAmmo;

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

	UPROPERTY(EditDefaultsOnly)
		float WeaponDamage = 20;

	FTimerHandle FireDelay;

	UFUNCTION()
		void ToggleFireDelay();

	UPROPERTY(EditDefaultsOnly)
		float RateOfFire = 1;

	UPROPERTY(Replicated)
		bool bIsFireDelayActive = false;

};
