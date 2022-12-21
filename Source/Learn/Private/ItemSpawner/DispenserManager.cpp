// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/DispenserManager.h"
#include "ItemSpawner/Dispenser.h"
#include "Containers/Queue.h"
#include "ItemSpawner/BaseItem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADispenserManager::ADispenserManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADispenserManager::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADispenser::StaticClass(), AllSites); // fill up array for all actors
	FTimerHandle ReSpawn;
	ItemCall();
	//GetWorld()->GetTimerManager().SetTimer(ReSpawn, this, &ADispenserManager::ItemCall, 5.0f, true);

}

void ADispenserManager::ItemCall()
{
	Items.Empty();
	for (int i{ 0 }; i < AllSites.Num(); i++)
	{
		ADispenser* Site = Cast<ADispenser>(AllSites[i]);
		Site->ItemSpawner();
		UE_LOG(LogTemp, Warning, TEXT("sites are spawning"));
		if (IsValid(Site->HolderItem))
		{
			Items.Enqueue(Site->HolderItem);
		}
	}
}

// Called every frame
void ADispenserManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

