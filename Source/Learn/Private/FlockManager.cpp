// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockManager.h"
#include "Bird.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFlockManager::AFlockManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AFlockManager::Initialize()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABird::StaticClass(),AllBirds);
	//cast to bird
}

// Called when the game starts or when spawned
void AFlockManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlockManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

