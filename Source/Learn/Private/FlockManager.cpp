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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABird::StaticClass(),AllBirds); // fill up array for all actors
	for (AActor* D : AllBirds) //fill up map to check if bird in a group
	{
		GroupChecker.Add(D, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("number of birds in map is :%d"), GroupChecker.Num());
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
	int mergeCount = 0; //we will use this to figure out the merge count of the birds

	//pase this out by logic depending on the size of the new flock
	//will scale from merge number of 2 to 4. 

	UE_LOG(LogTemp, Warning, TEXT("mergeflock size :%d"), NewFlock.Num());

	if (NewFlock.Num() == 2)
	{
		for (int i{ 0 }; i < NewFlock.Num(); i++)
		{
			midpoint += NewFlock[i]->GetActorLocation();

		}
		FVector rm = midpoint / 2;
		for (AActor* member : NewFlock)
		{
			//UE_LOG(LogTemp, Warning, TEXT("this member has been merged %s"), *member->GetName());
			ABird* bd = Cast<ABird>(member);
			APawn* Recasted = CastChecked<APawn>(member);
			ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
			FVector d = midpoint / 2;
			if (RC)
			{
				RC->MoveToLocation(d);
			}
			mergeCount += bd->MergeNum;
		}

		FTimerHandle DD;
		FTimerDelegate DestroyD = FTimerDelegate::CreateUObject(this, &AFlockManager::DestroyFlock, NewFlock, rm, mergeCount);
		GetWorld()->GetTimerManager().SetTimer(DD, DestroyD, 1.0, false);
	}
	else if (NewFlock.Num() == 3)
	{
		midpoint = (NewFlock[0]->GetActorLocation() + NewFlock[1]->GetActorLocation()) / 2; //get the midpoint between the first two birds
		midpoint = (midpoint + NewFlock[2]->GetActorLocation()) / 2; //midpoint between last bird and earlier midpoint. 
		for (AActor* member : NewFlock)
		{
			ABird* bd = Cast<ABird>(member);
			APawn* Recasted = CastChecked<APawn>(member);
			ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
			RC->MoveToLocation(midpoint);
			mergeCount += bd->MergeNum;
			//UE_LOG(LogTemp, Warning, TEXT("midpoint is :%s"), *midpoint.ToString());
			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Bird name is :%s"), Cast<FString>(*member->GetActorNameOrLabel()));
			//}
		}
		FTimerHandle DD;
		FTimerDelegate DestroyD = FTimerDelegate::CreateUObject(this, &AFlockManager::DestroyFlock, NewFlock, midpoint, mergeCount);
		GetWorld()->GetTimerManager().SetTimer(DD, DestroyD, 1.0, false);
	}
	else if (NewFlock.Num() == 4)
	{
		midpoint = (NewFlock[0]->GetActorLocation() + NewFlock[1]->GetActorLocation()) / 2;
		FVector midpoint2 = (NewFlock[2]->GetActorLocation() + NewFlock[3]->GetActorLocation()) / 2;
		midpoint = (midpoint + midpoint2) / 2; 
		for (AActor* member : NewFlock)
		{
			ABird* bd = Cast<ABird>(member);
			APawn* Recasted = CastChecked<APawn>(member);
			ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
			RC->MoveToLocation(midpoint);
			mergeCount += bd->MergeNum;
			//UE_LOG(LogTemp, Warning, TEXT("midpoint is :%s"), *midpoint.ToString());

			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Bird name is :%s"), Cast<FString>(*member->GetActorNameOrLabel()));
			//}
		}
		FTimerHandle DD;
		FTimerDelegate DestroyD = FTimerDelegate::CreateUObject(this, &AFlockManager::DestroyFlock, NewFlock, midpoint, mergeCount);
		GetWorld()->GetTimerManager().SetTimer(DD, DestroyD, 0.5, false);


	}

}

void AFlockManager::DestroyFlock(TArray<AActor*> NewFlock, FVector spawnloc, int CountNum)
{
	UE_LOG(LogTemp, Warning, TEXT("destroy flock"));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (AActor* member : NewFlock)
	{
		AllBirds.Remove(member);
		GroupChecker.Remove(member);
		member->Destroy();
	}

	ABigBird* TestBird = GetWorld()->SpawnActor<ABigBird>(BB, spawnloc, FRotator::ZeroRotator, SpawnParams);
	if (IsValid(TestBird))
	{
		TestBird->ScaleBird(CountNum);
	}
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
	bool counter = false;
	for (AActor* bird : RF)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actor in the array : %s"), *bird->GetName());
		if (IsValid(bird) && GroupChecker.Contains(bird) && GroupChecker[bird] == false)
		{
			GroupChecker[bird] = true;
			counter = true;
			UE_LOG(LogTemp, Warning, TEXT("wtf"));
		}
	}
	if (counter == true)
	{
		Cool.AddUnique(RF);
		UE_LOG(LogTemp, Warning, TEXT("unique array added"));
	}
}

void AFlockManager::tf()
{
	//call phases here
	if (AllBirds.Num() > 0)
	{
		for (AActor* birdie : AllBirds)
		{
			if (birdie != nullptr)
			{
				APawn* Recasted = Cast<APawn>(birdie);
				ABird* Bir = Cast<ABird>(birdie);
				ABirdController* R = Cast<ABirdController>(Recasted->GetController());
				//Bir->bcanDetect = true; //???maybe need. 
				//UE_LOG(LogTemp, Warning, TEXT("bcandetect is : %s"), (Bir->bcanDetect ? TEXT("true") : TEXT("false")));
				try
				{
					R->Flocking();
				}
				catch (const std::exception&)
				{
					UE_LOG(LogTemp, Warning, TEXT("issues with flocking"));
				}

			}

		}
	}
	if ( Cool.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("number of unique arrays in there :%d"), Cool.Num());
		for (TArray PR : Cool)
		{
			int length = PR.Num();
			//UE_LOG(LogTemp, Warning, TEXT("length of the array is :%d"), length);
			int randnum = FMath::RandRange(0, length);
			AActor* cl = PR[0];
			APawn* Recasted = CastChecked<APawn>(cl);
			ABirdController* BC = Cast<ABirdController>(Recasted->GetController());
			if (BC)
			{
				MergeFlock(BC->testarray);
				BC->testarray.Empty();
			}
		}
		Cool.Empty();
	}


}

void AFlockManager::chasePhase()
{
	//call phases here
	if (AllBirds.Num() > 0)
	{
		for (AActor* birdie : AllBirds)
		{
			if (birdie->IsA(ABird::StaticClass()))
			{
				APawn* Recasted = CastChecked<APawn>(birdie);
				ABird* Bir = Cast<ABird>(birdie);
				ABirdController* R = Cast<ABirdController>(Recasted->GetController());
				//Bir->bcanDetect = true; ???maybe need. 
				R->Chasing();
				UE_LOG(LogTemp, Warning, TEXT("birds are chasing"));
			}
		}
	}
}

void AFlockManager::evadePhase()
{
	//call phases here
	if (AllBirds.Num() > 0)
	{
		for (AActor* birdie : AllBirds)
		{
			if (birdie->IsA(ABird::StaticClass()))
			{
				APawn* Recasted = CastChecked<APawn>(birdie);
				ABird* Bir = Cast<ABird>(birdie);
				ABirdController* R = Cast<ABirdController>(Recasted->GetController());
				//Bir->bcanDetect = true; ???maybe need. 
				R->Evading();
				UE_LOG(LogTemp, Warning, TEXT("birds are evading"));
			}
		}
	}
}

void AFlockManager::Temp()
{
	pn = FMath::RandRange(0, 2);
	UE_LOG(LogTemp, Warning, TEXT("random number generator"));
}


// Called when the game starts or when spawned
void AFlockManager::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	pn = 0;

	FTimerHandle Testing;
	GetWorld()->GetTimerManager().SetTimer(Testing, this, &AFlockManager::Temp, 4.0f, true);

	FTimerHandle Update;
	GetWorld()->GetTimerManager().SetTimer(Update, this, &AFlockManager::tf, 1.0f,true); 
}

// Called every frame
void AFlockManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlockManager::StateUpdate()
{
	//listing all the states and calling them
	if (CurrentState == BirdState::IDLE)
	{
		if (CurrentAction == BirdAction::ON_ENTER)
		{
			Idle_Enter(); //if the state is idle and the action is enter idle, we enter the idle state;
		}
		else if (CurrentAction == BirdAction::ON_UPDATE)
		{
			Idle_Update();
		}

	}
	else if (CurrentState == BirdState::CHASE)
	{
		if (CurrentAction == BirdAction::ON_ENTER)
		{
			Chase_Enter();
		}
		if (CurrentAction == BirdAction::ON_UPDATE)
		{
			Chase_Update();
		}

	}
	else if (CurrentState == BirdState::MERGE)
	{
		if (CurrentAction == BirdAction::ON_ENTER)
		{
			Merge_Enter();
		}
		if (CurrentAction == BirdAction::ON_UPDATE)
		{
			Merge_Update();
		}

	}

}

void AFlockManager::SetBossState(BirdState newState)
{
	switch (CurrentState)
	{
	case BirdState::IDLE:
		break;
	case BirdState::CHASE:
		break;
	case BirdState::MERGE:
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("no state implemented"), newState)
		return;
	}
	CurrentState = newState;
	CurrentAction = BirdAction::ON_ENTER;
}

void AFlockManager::Idle_Enter()
{

	CurrentAction = BirdAction::ON_UPDATE;

}

void AFlockManager::Idle_Update()
{
	UE_LOG(LogTemp, Warning, TEXT("evading"));
	evadePhase();
	if (pn != 0)
	{
		Idle_Exit();
	}
}

void AFlockManager::Idle_Exit()
{
	if (pn == 1)
	{
		SetBossState(BirdState::CHASE);
	}
	if (pn == 2)
	{
		SetBossState(BirdState::MERGE);
	}

}

void AFlockManager::Chase_Enter()
{
	UE_LOG(LogTemp, Warning, TEXT("enter not update"));
	CurrentAction = BirdAction::ON_UPDATE;
}

void AFlockManager::Chase_Update()
{
	UE_LOG(LogTemp, Warning, TEXT("chasing"));
	chasePhase();
	if (pn != 1)
	{
		Chase_Exit();
	}
}

void AFlockManager::Chase_Exit()
{
	if (pn == 0)
	{
		SetBossState(BirdState::IDLE);
	}
	if (pn == 2)
	{
		SetBossState(BirdState::MERGE);
	}
}

void AFlockManager::Merge_Enter()
{

	CurrentAction = BirdAction::ON_UPDATE;
}

void AFlockManager::Merge_Update()
{
	tf();
	if (pn != 2)
	{
		Merge_Exit();
	}

}

void AFlockManager::Merge_Exit()
{
	if (pn == 0)
	{
		SetBossState(BirdState::IDLE);
	}
	if (pn == 1)
	{
		SetBossState(BirdState::CHASE);
	}
}
