// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENA_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int InventoryRifleAmmo = 30;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int InventoryRifleMagazineAmmo = 30;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int InventorySniperAmmo = 30;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int InventorySniperMagazineAmmo = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		TArray<TSubclassOf<class AWeapon>> InventoryWeapons;

};
