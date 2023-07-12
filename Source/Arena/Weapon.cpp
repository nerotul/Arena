// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/ArrowComponent.h"
#include "ArenaProjectile.h"
#include "GameFramework/Actor.h"
#include "ArenaCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DropObject.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	bReplicates = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterWeapon"));
	WeaponMesh->SetOnlyOwnerSee(true);
	RootComponent = WeaponMesh;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(WeaponMesh);

	ShellEjectDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ShellEjectDirection"));
	ShellEjectDirection->SetupAttachment(WeaponMesh);
	
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::Fire(FRotator InSpawnRotation)
{
	// try and fire a projectile
	if (ProjectileClass != nullptr && HasAuthority() && CurrentMagazineAmmo > 0)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = InSpawnRotation;
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AArenaProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		CurrentMagazineAmmo -= 1;
		MulticastOnFireFX();
	}
}

void AWeapon::MulticastOnFireFX_Implementation()
{
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (OwningCharacter->FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = OwningCharacter->Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(OwningCharacter->FireAnimation, 1.f);
		}
	}

	if (FXFire != nullptr)
	{
		FTransform Transform = MuzzleLocation->GetComponentTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, Transform);
	}

	if (ShellClass != nullptr)
	{
		const FVector SpawnLocation = ShellEjectDirection->GetComponentLocation();
		const FRotator SpawnRotation = ShellEjectDirection->GetComponentRotation();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADropObject* Shell = GetWorld()->SpawnActor<ADropObject>(ShellClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Shell)
		{
			Shell->SetLifeSpan(2.f);
		}
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, CurrentMagazineAmmo);
	DOREPLIFETIME(AWeapon, OwningCharacter);
}