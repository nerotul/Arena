// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaCharacter.h"
#include "ArenaProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "ArenaGameMode.h"
#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "ArenaPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AArenaCharacter

AArenaCharacter::AArenaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create third person gun mesh component
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->bCastDynamicShadow = false;
	TP_Gun->CastShadow = false;

	CharacterHealth = CreateDefaultSubobject<UHealthComponent>(TEXT("CharacterHealth"));
	CharacterHealth->OnCharacterDead.AddDynamic(this, &AArenaCharacter::ServerIncrementPlayerScore);

	CharacterInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("CharacterInventory"));
}

void AArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	TP_Gun->AttachToComponent(ACharacter::GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));

	// Show or hide the gun and hands based on whether or not pawn is locally controlled.
	
	if(HasAuthority())
	{
		WeaponClass = CharacterInventory->InventoryWeapons[InitialWeaponIndex];
		CurrentWeaponIndex = InitialWeaponIndex;
		ServerInitWeapon();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AArenaCharacter::ServerOnFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AArenaCharacter::ServerReloadWeapon);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AArenaCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArenaCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AArenaCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AArenaCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AArenaCharacter::ServerSwitchNextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AArenaCharacter::ServerSwitchPreviousWeapon);
	
}

void AArenaCharacter::MulticastSyncCameraPitch_Implementation(float InPitch)
{
	FRotator NewRotation = FirstPersonCameraComponent->GetComponentRotation();
	NewRotation.Pitch = InPitch;
	CharacterCameraRotation = NewRotation;
	FirstPersonCameraComponent->SetWorldRotation(NewRotation);

}

void AArenaCharacter::ServerSyncCameraPitch_Implementation(float InPitch)
{
	MulticastSyncCameraPitch(InPitch);
}

void AArenaCharacter::ServerSwitchNextWeapon_Implementation()
{
	if (CharacterInventory && CharacterWeapon->bCanReload)
	{
		if (CharacterInventory->InventoryWeapons.IsValidIndex(CurrentWeaponIndex + 1))
		{
			CharacterInventory->SetInventoryMagazineAmmo(CharacterWeapon->WeaponType, CharacterWeapon->CurrentMagazineAmmo);
			CharacterWeapon->Destroy();
			WeaponClass = CharacterInventory->InventoryWeapons[CurrentWeaponIndex + 1];
			CurrentWeaponIndex += 1;
			ServerInitWeapon();

		}
		else
		{
			CharacterInventory->SetInventoryMagazineAmmo(CharacterWeapon->WeaponType, CharacterWeapon->CurrentMagazineAmmo);
			CharacterWeapon->Destroy();
			WeaponClass = CharacterInventory->InventoryWeapons[0];
			CurrentWeaponIndex = 0;
			ServerInitWeapon();

		}
	}
}

void AArenaCharacter::ServerSwitchPreviousWeapon_Implementation()
{
	if (CharacterInventory && CharacterWeapon->bCanReload)
	{
		if (CharacterInventory->InventoryWeapons.IsValidIndex(CurrentWeaponIndex - 1))
		{
			CharacterInventory->SetInventoryMagazineAmmo(CharacterWeapon->WeaponType, CharacterWeapon->CurrentMagazineAmmo);
			CharacterWeapon->Destroy();
			WeaponClass = CharacterInventory->InventoryWeapons[CurrentWeaponIndex - 1];
			CurrentWeaponIndex -= 1;
			ServerInitWeapon();

		}
		else
		{
			CharacterInventory->SetInventoryMagazineAmmo(CharacterWeapon->WeaponType, CharacterWeapon->CurrentMagazineAmmo);
			CharacterWeapon->Destroy();
			int32 ArraySize = CharacterInventory->InventoryWeapons.Num();
			WeaponClass = CharacterInventory->InventoryWeapons[ArraySize - 1];
			CurrentWeaponIndex = (ArraySize - 1);
			ServerInitWeapon();

		}
	}
}

void AArenaCharacter::ServerInitWeapon_Implementation()
{
	if (WeaponClass)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Owner = this;

		FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);

		CharacterWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, ActorSpawnParams);

		if(CharacterWeapon)
		{
			CharacterWeapon->AttachToComponent(Mesh1P, Rule, FName("GripPoint"));
			CharacterWeapon->OwningCharacter = this;
			TP_Gun->SetSkeletalMesh(CharacterWeapon->TPWeaponMesh);
			CharacterWeapon->CurrentMagazineAmmo = CharacterInventory->GetInventoryMagazineAmmo(CharacterWeapon->WeaponType);


		}
	}
}

void AArenaCharacter::ServerOnFire_Implementation()
{
	if (bIsAlive && CharacterWeapon)
	{
		CharacterWeapon->Fire(GetControlRotation());

	}
}

void AArenaCharacter::ServerReloadWeapon_Implementation()
{
	if (bIsAlive && CharacterWeapon)
	{
		CharacterWeapon->ReloadWeapon();

	}
}


float AArenaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAlive)
	{
		float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		CharacterHealth->ChangeHealthValue(-DamageAmount);
		LastDamageCauser = DamageCauser;
		return ActualDamage;
	}
	else
	{
		return 0;
	}
}

void AArenaCharacter::MulticastKillCharacter_Implementation()
{
	bIsAlive = false;

	USkeletalMeshComponent* TP_Mesh = ACharacter::GetMesh();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TP_Mesh->SetSimulatePhysics(true);
	TP_Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	CharacterWeapon->Destroy();
	Mesh1P->DestroyComponent();
	GetWorldTimerManager().SetTimer(DestroyActorHandle, this, &AArenaCharacter::CallDestroy, 5.0f, false);

	if (HasAuthority())
	{
		AGameModeBase* GM = GetWorld()->GetAuthGameMode();
		if (AArenaGameMode* GameMode = Cast<AArenaGameMode>(GM))
		{
			AController* Test = GetController();
			GameMode->RespawnCharacter(GetController());
		}
	}
}

void AArenaCharacter::CallDestroy()
{
	Destroy();
}

void AArenaCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AArenaCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AArenaCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AArenaCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

	// Camera pitch synchronization between players
	if (HasAuthority())
	{
		MulticastSyncCameraPitch(FirstPersonCameraComponent->GetComponentRotation().Pitch);
	}
	else
	{
		ServerSyncCameraPitch(FirstPersonCameraComponent->GetComponentRotation().Pitch);
	}
}


void AArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaCharacter, CharacterWeapon);

}

void AArenaCharacter::ServerIncrementPlayerScore_Implementation()
{
	if (LastDamageCauser != nullptr)
	{
		AArenaCharacter* KillerPlayer = Cast<AArenaCharacter>(LastDamageCauser);
		AArenaPlayerState* PS = Cast<AArenaPlayerState>(KillerPlayer->GetPlayerState());
		PS->IncrementPlayerScore();

	}

	MulticastKillCharacter();
}
