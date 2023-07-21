// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveObject.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AExplosiveObject::AExplosiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
}

// Called when the game starts or when spawned
void AExplosiveObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AExplosiveObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ObjectHealth -= DamageAmount;
	LastDamageCauser = DamageCauser;

	if (ObjectHealth < 0)
	{
		Explode();
	}
	
	return DamageAmount;
}

void AExplosiveObject::Explode()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, NULL, IgnoredActors, LastDamageCauser, nullptr, true);

	MulticastExplodeFX();
}

void AExplosiveObject::MulticastExplodeFX_Implementation()
{
	if (ExplosionSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	if (ExplosionEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	HasAuthority() ? Destroy() : NULL;
	
}

void AExplosiveObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplosiveObject, ObjectHealth);

}
