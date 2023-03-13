// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawn.h"
#include "Components/BoxComponent.h"
#include "Points.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bird.h"
#include "BirdController.h"
#include "FlockManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpawn::ASpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Volume"));
	SetRootComponent(SpawnVolume);

}

FVector ASpawn::GetPoint()
{
	FVector SpawnOrigin = SpawnVolume->Bounds.Origin;
	FVector SpawnExtent = SpawnVolume->Bounds.BoxExtent;

	FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
	return SpawnPoint;
}

void ASpawn::SpawnPoints()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnPoint = GetPoint();
	APoints* DatPoint = GetWorld()->SpawnActor<APoints>(Point, SpawnPoint, FRotator::ZeroRotator, SpawnParams);

}

void ASpawn::SpawnBirds()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	if (Manager->AllBirds.Num() <= 10)
	{
		FVector SpawnPoint = GetPoint();
		ABird* NewBird = GetWorld()->SpawnActor<ABird>(Birdie, SpawnPoint, FRotator::ZeroRotator, SpawnParams);
		ABirdController* brain = Cast<ABirdController>(NewBird->GetController());
		brain->Manager->GroupChecker.Add(NewBird, false);
		brain->Manager->AllBirds.Add(NewBird);
	}
	//spawn birds in the box and 
}


// Called when the game starts or when spawned
void ASpawn::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle Spawn;
	GetWorld()->GetTimerManager().SetTimer(Spawn, this, &ASpawn::SpawnPoints, 2.0f, true);

	FTimerHandle SpawnBirds;
	GetWorld()->GetTimerManager().SetTimer(SpawnBirds, this, &ASpawn::SpawnBirds, 5.0f, true);

	Manager = Cast<AFlockManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFlockManager::StaticClass()));
	
}

// Called every frame
void ASpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

