// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/Dispenser.h"

// Sets default values
ADispenser::ADispenser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);

}

// Called when the game starts or when spawned
void ADispenser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADispenser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

