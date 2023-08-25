// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArenaInterface.h"
#include "ArenaCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class UHealthComponent;
class UInventoryComponent;

UCLASS(config=Game)
class AArenaCharacter : public ACharacter, public IArenaInterface
{
	GENERATED_BODY()


public:
	AArenaCharacter();

	//Gun mesh for third person
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* TP_FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

protected:
	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void ServerOnFire();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastKillCharacter();

	void CallDestroy();

	FTimerHandle DestroyActorHandle;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

		
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	UPROPERTY(BlueprintReadOnly)
		bool bIsAlive = true;

	UPROPERTY(BlueprintReadOnly)
		UHealthComponent* CharacterHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UInventoryComponent* CharacterInventory;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(Replicated, BlueprintReadOnly)
		AWeapon* CharacterWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* FP_ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* TP_ReloadAnimation;

	UPROPERTY(EditDefaultsOnly)
		int InitialWeaponIndex = 0;

	UPROPERTY()
		int CurrentWeaponIndex;


	UFUNCTION(Server, Reliable)
		void ServerInitWeapon();

	UFUNCTION(Server, Reliable)
		void ServerReloadWeapon();

	UFUNCTION(Server, Reliable)
		void ServerSwitchNextWeapon();

	UFUNCTION(Server, Reliable)
		void ServerSwitchPreviousWeapon();

	UFUNCTION(Server, Unreliable)
		void ServerSyncCameraPitch(float InPitch);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastSyncCameraPitch(float InPitch);

	UPROPERTY(BlueprintReadOnly)
		FRotator CharacterCameraRotation;

	UFUNCTION(Server, Reliable)
		void ServerIncrementPlayerScore();

	UPROPERTY(BlueprintReadOnly)
		AController* LastDamageInstigator;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RespawnCharacter();

	FTimerHandle RespawnCharacterHandle;

	UPROPERTY(EditDefaultsOnly)
		float BodyDisappearanceDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly)
		float RespawnCharacterDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly)
		bool bIsNPC = false;

		bool bIsFireInputPressed = false;

	UFUNCTION(Server, Unreliable)
	void ServerFireInputPressed();

	UFUNCTION(Server, Unreliable)
	void ServerFireInputReleased();

};

