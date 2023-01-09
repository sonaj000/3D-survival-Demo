// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner/BaseItem.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Bird.h"
#include "MCharacter.h"
#include "ItemSpawner/DispenserManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	BoxComp->SetupAttachment(RootComponent);

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::BeginOverLap);

}

void ABaseItem::PowerUp(AActor* MCharacter)
{
	Destroy();
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	DM = UGameplayStatics::GetActorOfClass(GetWorld(), ADispenserManager::StaticClass());
	
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::BeginOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//since both birds and characters can destory items, check if the other actor is a character first. 
	if (IsValid(OtherActor) && OtherActor != this && OtherActor->IsA(AMCharacter::StaticClass())) //&& OtherActor->IsA(ACharacter::StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("powerup overlap"));
		ADispenserManager* Site = Cast<ADispenserManager>(DM);
		ABaseItem* tail;
		Site->Items.Dequeue(tail);
		PowerUp(OtherActor);
	}
	else if (IsValid(OtherActor) && OtherActor != this && OtherActor->IsA(ABird::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("destroy"));
		ADispenserManager* Site = Cast<ADispenserManager>(DM);
		ABaseItem* tail;
		Site->Items.Dequeue(tail);
		Destroy();
	}
	else
	{
		return;
	}
}

void ABaseItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

