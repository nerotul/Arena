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
#include "InventoryComponent.h"
#include "Engine/SkeletalMeshSocket.h"


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

	MagazineEjectDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MagazineEjectDirection"));
	MagazineEjectDirection->SetupAttachment(WeaponMesh);
	
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

//Called in Character on server
void AWeapon::Fire(FRotator InSpawnRotation)
{
	// try and fire a projectile
	if (ProjectileClass != nullptr && HasAuthority() && CurrentMagazineAmmo > 0 && bCanFire)
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

	if (OwningCharacter->TP_FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(OwningCharacter->TP_FireAnimation, 1.f);
		}

	}

	if (FXFire != nullptr)
	{
		//FX with scene component
		//FTransform Transform = MuzzleLocation->GetComponentTransform();
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, Transform);

		//FX with socket on weapon
		const USkeletalMeshSocket* Socket = WeaponMesh->GetSocketByName("MuzzleFlash");
		FTransform TransformTest = Socket->GetSocketTransform(WeaponMesh);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, TransformTest);

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

//Called in Character on server
void AWeapon::ReloadWeapon()
{
	if (bCanReload)
	{
		int MagazineEmptySpace = MaxMagazineAmmo - CurrentMagazineAmmo;
		int& InventoryAmmo = OwningCharacter->CharacterInventory->InventoryRifleAmmo;

		if (MagazineEmptySpace > 0 && InventoryAmmo != 0)
		{
			bCanFire = false;
			bCanReload = false;

			if (InventoryAmmo > MagazineEmptySpace)
			{
				CurrentMagazineAmmo += MagazineEmptySpace;
				InventoryAmmo -= MagazineEmptySpace;
				MulticastReloadFX();

			}
			else
			{
				CurrentMagazineAmmo += InventoryAmmo;
				InventoryAmmo = 0;
				MulticastReloadFX();

			}
		}

	}
}

void AWeapon::MulticastReloadFX_Implementation()
{
	if (ReloadSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetActorLocation());
	}
	
	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr)
	{
		AnimInstance->Montage_Play(OwningCharacter->TP_ReloadAnimation, 1.f);
		float AnimLength = OwningCharacter->TP_ReloadAnimation->GetPlayLength();
		GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AWeapon::ServerToggleReloadRestrictions, AnimLength, false);

	}

	if (MagazineClass != nullptr)
	{
		const FVector SpawnLocation = MagazineEjectDirection->GetComponentLocation();
		const FRotator SpawnRotation = MagazineEjectDirection->GetComponentRotation();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADropObject* Magazine = GetWorld()->SpawnActor<ADropObject>(MagazineClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Magazine)
		{
			Magazine->SetLifeSpan(2.f);
		}
	}

}

void AWeapon::ServerToggleReloadRestrictions_Implementation()
{
	bCanFire = true;
	bCanReload = true;

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, CurrentMagazineAmmo);
	DOREPLIFETIME(AWeapon, OwningCharacter);
	DOREPLIFETIME(AWeapon, bCanFire);
	DOREPLIFETIME(AWeapon, bCanReload);
}