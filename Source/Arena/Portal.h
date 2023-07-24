// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaInterface.h"
#include "Portal.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ATargetPoint;

UCLASS()
class ARENA_API APortal : public AActor, public IArenaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
		UBoxComponent* CollisionBox = nullptr;;

	UPROPERTY(EditAnywhere)
		ATargetPoint* PortalToLocation = nullptr;

	virtual void Interact(AActor* Interactor) override;

};
