// Fill out your copyright notice in the Description page of Project Settings.


#include "Bird.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "BirdController.h"
#include "Kismet/GameplayStatics.h"
#include "BigBird.h"

// Sets default values
ABird::ABird()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BirdMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("cool"));
	BirdMesh->SetupAttachment(RootComponent);

	FlockPerimeter = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	FlockPerimeter->SetupAttachment(RootComponent);

	FlockPerimeter->OnComponentBeginOverlap.AddDynamic(this, &ABird::BeginOverLap);
	FlockPerimeter->OnComponentEndOverlap.AddDynamic(this, &ABird::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	Remote = Cast<ABirdController>(this->GetController());
	bcanDetect = true;
	
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABird::BeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Remote && !OtherActor->IsA(ABigBird::StaticClass()))
	{
		if  (bcanDetect && Remote->bisFlocking && Remote->testarray.Num() <= 4) //do not add more birds if flock max is 4. add this bnack Remote->bisFlocking
		{
			if (OtherActor != this  && !Remote->testarray.Contains(OtherActor)) //need to double check that big birds do not get added so they do not merge
			{
				Remote->testarray.AddUnique(OtherActor);
				UE_LOG(LogTemp, Warning, TEXT("actor added :%s"), *OtherActor->GetName());
				bcanDetect = false;
			}
		}
	}

}

void ABird::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Remote)
	{
		if (!bcanDetect && Remote->bisFlocking)
		{
			bcanDetect = true;
			UE_LOG(LogTemp, Warning, TEXT("bcandetect reset"));
		}
	}
}


