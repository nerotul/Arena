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

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Create third person gun mesh component
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->bCastDynamicShadow = false;
	TP_Gun->CastShadow = false;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	CharacterHealth = CreateDefaultSubobject<UHealthComponent>(TEXT("CharacterHealth"));
	CharacterHealth->OnCharacterDead.AddDynamic(this, &AArenaCharacter::MulticastKillCharacter);

	CharacterInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("CharacterInventory"));
}

void AArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	TP_Gun->AttachToComponent(ACharacter::GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));

	// Show or hide the gun and hands based on whether or not pawn is locally controlled.
	InitWeapon();
	SetMeshVisibility();
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
	
	
	// Calling it here because BeginPlay is too fast when respawning Listen-Server
	SetMeshVisibility();
}

void AArenaCharacter::InitWeapon()
{
	if (WeaponClass)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);

		CharacterWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, ActorSpawnParams);

		if(CharacterWeapon)
		{
			CharacterWeapon->AttachToComponent(Mesh1P, Rule, FName("GripPoint"));
			//FP_Gun->SetSkeletalMesh(CharacterWeapon->FPWeaponMesh);
			TP_Gun->SetSkeletalMesh(CharacterWeapon->FPWeaponMesh);

		}
	}
}

void AArenaCharacter::SetMeshVisibility()
{
	if (IsLocallyControlled())
	{
		Mesh1P->SetHiddenInGame(false, true);
		FP_Gun->SetHiddenInGame(false, true);
		TP_Gun->SetHiddenInGame(true, true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(true, true);
		FP_Gun->SetHiddenInGame(true, true);
		TP_Gun->SetHiddenInGame(false, true);
	}
}

void AArenaCharacter::ServerOnFire_Implementation()
{
	if (bIsAlive && CharacterInventory->AvailableAmmo > 0)
	{
		CharacterInventory->AvailableAmmo -= 1;

		//MulticastOnFireFX();

		CharacterWeapon->Fire(GetControlRotation());
	}
}

void AArenaCharacter::MulticastOnFireFX_Implementation()
{
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}


}

float AArenaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAlive)
	{
		float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		CharacterHealth->ChangeHealthValue(-DamageAmount);

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
	Mesh1P->DestroyComponent();
	FP_Gun->DestroyComponent();
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
	CharacterWeapon->Destroy();
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
}
