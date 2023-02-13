// Fill out your copyright notice in the Description page of Project Settings.


#include "MergePoint.h"
#include "Components/BoxComponent.h"
#include "Bird.h"
#include "BirdController.h"
#include "FlockManager.h"
#include "BigBird.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMergePoint::AMergePoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;

	MergePerimeter = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Perimeter"));
	MergePerimeter->SetupAttachment(RootComponent);

	MergePerimeter->OnComponentBeginOverlap.AddDynamic(this, &AMergePoint::BeginOverLap);

	Counter = 0;

}

// Called when the game starts or when spawned
void AMergePoint::BeginPlay()
{
	Super::BeginPlay();

	Manager = Cast<AFlockManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFlockManager::StaticClass()));

}

void AMergePoint::BeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("overlap with strucutre"));
	if (IsValid(OtherActor) && OtherActor != this && OtherActor->IsA(ABird::StaticClass()) && Manager->AllBirds.Num() < Manager->MaxBirds)
	{
		ABird* testbird = Cast<ABird>(OtherActor);
		ABirdController* birdbrain = Cast<ABirdController>(testbird->GetController());
		if (testbird->MergeNum < 4 && birdbrain->bisFlocking)
		{
			MergerQueue.Enqueue(OtherActor);
			Counter += 1;
			UE_LOG(LogTemp, Warning, TEXT("counter is :%d"), Counter);
			if (Counter - 2 == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("merging"));
				MergeBirds();
				Counter = 0;
			}
		}

	}
}

void AMergePoint::MergeBirds()
{
	AActor* Bird0;
	//UE_LOG(LogTemp, Warning, TEXT("mergeflock"));
	FVector midpoint = FVector(0, 0, 0);
	int mergeCount = 0;

	if (MergerQueue.Dequeue(Bird0))
	{
		UE_LOG(LogTemp, Warning, TEXT("dequeued 1"));
		midpoint = Bird0->GetActorLocation();
	}
	ABird* bd = Cast<ABird>(Bird0);

	if (MergerQueue.Dequeue(Bird0))
	{
		midpoint = Bird0->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("dequeued 2"));
	}
	ABird* bd1 = Cast<ABird>(Bird0);
	mergeCount = bd->MergeNum + bd1->MergeNum;
	UE_LOG(LogTemp, Warning, TEXT("mergecount : %d"), mergeCount);
	if (bd != nullptr && bd1 != nullptr && mergeCount <= 4)
	{
		//UE_LOG(LogTemp, Warning, TEXT("can move the people"));
		APawn* Recasted = CastChecked<APawn>(bd);
		ABirdController* RC = Cast<ABirdController>(Recasted->GetController());

		APawn* Recasted1 = CastChecked<APawn>(bd1);
		ABirdController* RC1 = Cast<ABirdController>(Recasted1->GetController());

		FVector d = Manager->GetActorLocation();

		if (IsValid(RC) && IsValid(RC1))
		{
			RC->MoveToActor(this);
			RC1->MoveToActor(this);

			CreateBird(bd, bd1, d, mergeCount);
		}
	}
}

void AMergePoint::CreateBird(ABird* bird0, ABird* bird1, FVector spawnloc, int CountNum)
{
	UE_LOG(LogTemp, Warning, TEXT("create new bird"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Manager->AllBirds.Remove(bird0);
	Manager->AllBirds.Remove(bird1);

	bird0->Destroy();
	bird1->Destroy();


	ABigBird* TestBird = GetWorld()->SpawnActor<ABigBird>(BigB, spawnloc, FRotator::ZeroRotator, SpawnParams);
	if (IsValid(TestBird))
	{
		TestBird->ScaleBird(CountNum);
		TestBird->bcanDetect = true;
	}
	Manager->AllBirds.AddUnique(TestBird);

	//UE_LOG(LogTemp, Warning, TEXT("spawn big bird loc:%s"), *spawnloc.ToString());
}

// Called every frame
void AMergePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

