// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockManager.h"
#include "Bird.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BirdController.h"
#include "BigBird.h"

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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("all the birds are added"));
	}
	
}

void AFlockManager::MergeFlock(TArray<AActor*> NewFlock)
{
	FVector midpoint = FVector(0, 0, 0);
	UE_LOG(LogTemp, Warning, TEXT("mergeflock size :%d"), NewFlock.Num());
	for (int i{ 0 }; i < NewFlock.Num(); i++)
	{
		midpoint += NewFlock[i]->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("locations are :%s"), *midpoint.ToString());
	}
	for (AActor* member : NewFlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("this member has been merged %s"), *member->GetName());
		APawn*Recasted = CastChecked<APawn>(member);
		ABirdController*RC = Cast<ABirdController>(Recasted->GetController());
		FVector d = midpoint / 2;
		RC->MoveToLocation(d);
		UE_LOG(LogTemp, Warning, TEXT("midpoint is :%s"), *d.ToString());

		UE_LOG(LogTemp, Warning, TEXT("move to midpoint"));


		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Bird name is :%s"), Cast<FString>(*member->GetActorNameOrLabel()));
		//}
	}
	FTimerHandle DD;
	FTimerDelegate DestroyD = FTimerDelegate::CreateUObject(this, &AFlockManager::DestroyFlock, NewFlock,midpoint);
	GetWorld()->GetTimerManager().SetTimer(DD, DestroyD, 1.5f, false);
}

void AFlockManager::DestroyFlock(TArray<AActor*> NewFlock, FVector spawnloc)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (AActor* member : NewFlock)
	{
		AllBirds.Remove(member);
		member->Destroy();
	}
	//spawn big bird at midpoint
	GetWorld()->SpawnActor<ABigBird>(SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("spawn big bird"));

}




// Called when the game starts or when spawned
void AFlockManager::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	
}

// Called every frame
void AFlockManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

