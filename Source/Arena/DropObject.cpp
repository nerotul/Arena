// Fill out your copyright notice in the Description page of Project Settings.


#include "DropObject.h"

// Sets default values
ADropObject::ADropObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shell"));
}

// Called when the game starts or when spawned
void ADropObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADropObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

