// Copyright Epic Games, Inc. All Rights Reserved.


#include "LearnGameModeBase.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Points.h"

ALearnGameModeBase::ALearnGameModeBase()
{
	TotalScore = 100;
}

void ALearnGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	//FTimerHandle Spawn;
	//GetWorld()->GetTimerManager().SetTimer(Spawn, this, &ALearnGameModeBase::SpawnPoint, 5.0f, true);
}

void ALearnGameModeBase::SpawnPoint()
{
	UNavigationSystemV1* NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FNavLocation ResultLocation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	bool bSuccess = NavArea->GetRandomPoint(ResultLocation);
	FVector translate = ResultLocation;
	translate.Z = 200.0f;
	if (bSuccess)
	{
		APoints* Point = GetWorld()->SpawnActor<APoints>(PointItem, translate, FRotator::ZeroRotator, SpawnParams);
		UE_LOG(LogTemp, Warning, TEXT("points spawned"));
	}

}
