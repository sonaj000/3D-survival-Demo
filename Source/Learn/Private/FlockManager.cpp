// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockManager.h"
#include "Bird.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BirdController.h"
#include "BigBird.h"
#include "Math/UnrealMathUtility.h"


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
	UE_LOG(LogTemp, Warning, TEXT("mergeflock"));
	FVector midpoint = FVector(0, 0, 0);
	UE_LOG(LogTemp, Warning, TEXT("mergeflock size :%d"), NewFlock.Num());
	for (int i{ 0 }; i < NewFlock.Num(); i++)
	{
		midpoint += NewFlock[i]->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("locations are :%s"), *midpoint.ToString());
	}
	FVector rm = midpoint / 2;
	for (AActor* member : NewFlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("this member has been merged %s"), *member->GetName());
		APawn*Recasted = CastChecked<APawn>(member);
		ABirdController*RC = Cast<ABirdController>(Recasted->GetController());
		FVector d = midpoint / 2;
		RC->MoveToLocation(d);
		UE_LOG(LogTemp, Warning, TEXT("midpoint is :%s"), *d.ToString());


		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Bird name is :%s"), Cast<FString>(*member->GetActorNameOrLabel()));
		//}
	}
	FTimerHandle DD;
	FTimerDelegate DestroyD = FTimerDelegate::CreateUObject(this, &AFlockManager::DestroyFlock, NewFlock,rm);
	GetWorld()->GetTimerManager().SetTimer(DD, DestroyD, 1.5, false);

}

void AFlockManager::DestroyFlock(TArray<AActor*> NewFlock, FVector spawnloc)
{
	UE_LOG(LogTemp, Warning, TEXT("destroy flock"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (AActor* member : NewFlock)
	{
		AllBirds.Remove(member);
		member->Destroy();
	}


	ABigBird* TestBird = GetWorld()->SpawnActor<ABigBird>(BB, spawnloc, FRotator::ZeroRotator, SpawnParams);
	AllBirds.AddUnique(TestBird);

	UE_LOG(LogTemp, Warning, TEXT("spawn big bird loc:%s"), *spawnloc.ToString());

	//
	//FTransform ReturnTransform;
	//ReturnTransform.SetLocation(FVector(-20170.0, -6710.0, 100));
	//ReturnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());

	//ABird* t = GetWorld()->SpawnActor<ABird>(MyActorClass, ReturnTransform);

}

void AFlockManager::CheckUnique(TArray<AActor*>RF)
{
	Cool.AddUnique(RF);
}

void AFlockManager::tf()
{
	/* so basically we will go through our array of pointers to arrays made up of pointers to actors and for every pointer in cool(pointer to an array), we will randomly pick a pointer(to an actor) in that array
	 and recast it to a pawn, get the controller aka whatever we are doing already in mergeflock and then call upon the mergeflock function. */
	if ( Cool.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("number of unique arrays in there :%d"), Cool.Num());
		for (TArray PR : Cool)
		{
			int length = PR.Num();
			UE_LOG(LogTemp, Warning, TEXT("length of the array is :%d"), length);
			int randnum = FMath::RandRange(0, length);
			UE_LOG(LogTemp, Warning, TEXT("randnum :%d"), randnum);
			AActor* cl = PR[0];
			APawn* Recasted = CastChecked<APawn>(cl);
			ABirdController* BC = Cast<ABirdController>(Recasted->GetController());
			MergeFlock(BC->testarray);
		}
	}
	Cool.Empty();

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

