// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Components/BoxComponent.h"
#include "Engine/TargetPoint.h"
#include "ArenaCharacter.h"
#include "Components/PointLightComponent.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Component"));
	RootComponent = CollisionBox;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light Component"));
	PointLight->SetupAttachment(CollisionBox);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetupAttachment(CollisionBox);

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::Interact(AActor* Interactor)
{
	if (IsValid(Interactor))
	{
		AArenaCharacter* OverlappedCharacter = Cast<AArenaCharacter>(Interactor);

		if (IsValid(OverlappedCharacter))
		{
			FVector TravelLocation = PortalToLocation->GetActorLocation();
			FRotator TravelRotation = PortalToLocation->GetActorRotation();
			OverlappedCharacter->SetActorRotation(TravelRotation);
			OverlappedCharacter->SetActorLocation(TravelLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}

}

