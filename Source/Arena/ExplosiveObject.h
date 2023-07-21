// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveObject.generated.h"

UCLASS()
class ARENA_API AExplosiveObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = FX)
		USoundBase* ExplosionSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = FX)
		UParticleSystem* ExplosionEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Health, Replicated)
		float ObjectHealth = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
		float ExplosionDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
		float ExplosionRadius = 100.0f;

	UPROPERTY(BlueprintReadOnly)
		AActor* LastDamageCauser = nullptr;


	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void Explode();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastExplodeFX();

};
