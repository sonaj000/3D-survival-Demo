// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/Dispenser.h"
#include "ItemSpawner/BaseItem.h"
#include "ItemSpawner/AttackUp.h"
#include "ItemSpawner/HealthUp.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ADispenser::ADispenser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);

	ReferencePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Reference point"));

	HolderItem = NULL;
}

// Called when the game starts or when spawned
void ADispenser::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADispenser::ItemSpawner()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnPoint = ReferencePoint->GetComponentTransform().GetLocation();

	int itemID = FMath::RandRange(0, 1);
	UE_LOG(LogTemp, Warning, TEXT("int"));

	switch (itemID)
	{
		case 0:
			HolderItem = GetWorld()->SpawnActor<AAttackUp>(AttackItem, this->GetActorLocation() + FVector(0, 0, -400), FRotator::ZeroRotator, SpawnParams);
			UE_LOG(LogTemp, Warning, TEXT("attack"));
			break;
		case 1:
			HolderItem = GetWorld()->SpawnActor<AHealthUp>(HealthItem, this->GetActorLocation() + FVector(0,0,-400), FRotator::ZeroRotator, SpawnParams);
			UE_LOG(LogTemp, Warning, TEXT("health"));
			break;
	}
	UE_LOG(LogTemp, Warning, TEXT("finished"));

}



// Called every frame
void ADispenser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

