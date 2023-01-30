// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockManager.h"
#include "Bird.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BirdController.h"
#include "BigBird.h"
#include "Math/UnrealMathUtility.h"
#include <stdio.h>
#include "FNN.h"
#include "ItemSpawner/BaseItem.h"
#include "MCharacter.h"



// Sets default values
AFlockManager::AFlockManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFlockManager::Initialize()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABird::StaticClass(), AllBirds); // fill up array for all actors
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
	Mainchar = Cast<AMCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMCharacter::StaticClass()));
	NeuralNetwork = Cast<AFNN>(UGameplayStatics::GetActorOfClass(GetWorld(), AFNN::StaticClass()));
	NeuralNetwork->Initialize(7, 6, 4);
	NeuralNetwork->SetLearningRate(0.2);
	NeuralNetwork->SetMomentum(true, 0.9);

	// Size1	Size2	Size3	Size4	Health	PHealth			Item	Evade	Chase	Merge	Destroy	
	TrainingSet = {
	{ 0,		1.0,	0,		0,		1,		1,				0,			0,		0,		0,		1.0 },
	{1,			0.0,	0,		0,		0.5,	1,				0,			0,		0,		1.0,	0 },
	{0,			1,		0,		0, 		0.6, 	0.5,			0, 			0,		0.5, 	0.5,	0 },
	{0,			0,		0,		1,		0.8,	1,				1,			0,		1.0,	0,		0},
	{0,			1,		0,		0,		1,		1,				1,			0,		0.0,	0.5,	0.5},
	{1,			0,		0,		0,		1,		0.5,			0,			0,		0.1,	0.9,	0.1},
	{1,			0,		0,		0,		0.5,	1,				0,			0.5,	0.0,	0.5,	0},
	{0,			1,		0,		0,		1,		1,				0,			0.0,	0.1,	0.9,	0},
	{0,			0,		1,		0,		0.75,	1,				0,			0.1,	0.9,	0.0,	0.1},
	{1,			0,		0,		0,		0,		1,				1,			0.0,	0,		0.1,	0.9},
	{1,			0,		0,		0,		0,		1,				1,			0.0,	0,		0.1,	0.9},
	{1,			0,		0,		0,		0.5,	1,				0,			0,		0,		0.9,	0.1},
	{1,			0,		0,		0,		1,		1,				1,			0.0,	0,		0.1,	0.9},
	{0,			1,		0,		0,		0.3,	1,				1,			0.0,	0,		0.1,	0.9},
	{1,			0,		0,		0,		1,		1,				1,			0,		0,		0.9,	0.1},
	};

	UE_LOG(LogTemp, Warning, TEXT("TrainSet index is :%f"), TrainingSet[2][1]);

}

void AFlockManager::MergeFlock(TArray<AActor*> NewFlock)
{
	UE_LOG(LogTemp, Warning, TEXT("mergeflock"));
	FVector midpoint = FVector(0, 0, 0);
	int mergeCount = 0; //we will use this to figure out the merge count of the birds

	//pase this out by logic depending on the size of the new flock
	//will scale from merge number of 2 to 4. 
	//check to see if any birds are null
	for (AActor* check : NewFlock)
	{
		if (check == nullptr)
		{
			NewFlock.Remove(check);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("mergeflock size :%d"), NewFlock.Num());
	if (NewFlock.Num() == 2)
	{
		for (int i{ 0 }; i < NewFlock.Num(); i++)
		{
			if (IsValid(NewFlock[i]))
			{
				midpoint += NewFlock[i]->GetActorLocation();
			}

		}
		FVector rm = midpoint / 2;
		for (AActor* member : NewFlock)
		{
			//UE_LOG(LogTemp, Warning, TEXT("this member has been merged %s"), *member->GetName());
			ABird* bd = Cast<ABird>(member);
			if (bd != nullptr)
			{
				APawn* Recasted = CastChecked<APawn>(member);
				ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
				FVector d = midpoint / 2;
				if (RC)
				{
					RC->MoveToLocation(d);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("not moving to location"));
				}
				if (IsValid(bd))
				{
					mergeCount += bd->MergeNum;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("merge number not valid"));
				}
			}

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
			if (bd != nullptr)
			{
				APawn* Recasted = CastChecked<APawn>(member);
				ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
				RC->MoveToLocation(midpoint);
				if (IsValid(bd))
				{
					mergeCount += bd->MergeNum;
				}
			}

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
			if (bd != nullptr)
			{
				APawn* Recasted = CastChecked<APawn>(member);
				ABirdController* RC = Cast<ABirdController>(Recasted->GetController());
				RC->MoveToLocation(midpoint);
				if (IsValid(bd))
				{
					mergeCount += bd->MergeNum;
				}
			}

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
	GroupChecker.Add(TestBird, false);
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
		if (IsValid(bird))
		{
			UE_LOG(LogTemp, Warning, TEXT("bird is valid in the array being checked"));
			if (GroupChecker.Contains(bird))
			{
				UE_LOG(LogTemp, Warning, TEXT("group chcker contains bird"));
				if (GroupChecker[bird] == false)
				{
					GroupChecker[bird] = true;
					counter = true;
					UE_LOG(LogTemp, Warning, TEXT("bird is true somehow"));
				}

			}
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
				R->Flocking();

			}
		}
	}
	if (Cool.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("number of unique arrays in there :%d"), Cool.Num());
		for (TArray PR : Cool)
		{
			int length = PR.Num();
			int randnum = FMath::RandRange(0, length);
			AActor* cl = PR[0];
			if (IsValid(cl))
			{
				UE_LOG(LogTemp, Warning, TEXT("length of the array is :%d"), length);
				APawn* Recasted = Cast<APawn>(cl);
				ABirdController* BC = Cast<ABirdController>(Recasted->GetController());
				if (IsValid(BC))
				{
					MergeFlock(BC->testarray);
					BC->testarray.Empty(); // help reset it
				}
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
				R->testarray.Empty();
				for (int i{ 0 }; i < GroupChecker.Num(); i++)
				{
					GroupChecker[AllBirds[i]] = false;
				}
				R->Chasing();
				//UE_LOG(LogTemp, Warning, TEXT("birds are chasing"));
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
				R->testarray.Empty();
				for (int i{ 0 }; i < GroupChecker.Num(); i++)
				{
					GroupChecker[AllBirds[i]] = false;
				}
				R->Evading();
				//UE_LOG(LogTemp, Warning, TEXT("birds are evading"));
			}
		}
	}
}

void AFlockManager::DestroyPhase()
{
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
				R->testarray.Empty();
				for (int i{ 0 }; i < GroupChecker.Num(); i++)
				{
					GroupChecker[AllBirds[i]] = false;
				}
				R->Destroying();
				//UE_LOG(LogTemp, Warning, TEXT("birds are evading"));
			}
		}
	}
}

void AFlockManager::Temp()
{
	pn = FMath::RandRange(0, 3);
	//UE_LOG(LogTemp, Warning, TEXT("random number generator"));
}



// Called when the game starts or when spawned
void AFlockManager::BeginPlay()
{
	Super::BeginPlay();

	Initialize();

	//ImportData(); need to fix
	StartData();

	pn = 0;

	FTimerHandle Testing;
	GetWorld()->GetTimerManager().SetTimer(Testing, this, &AFlockManager::newCommand, 5.0f, true);

	FTimerHandle Update;
	GetWorld()->GetTimerManager().SetTimer(Update, this, &AFlockManager::StateUpdate, 1.0f, true);

	FTimerHandle Update2;
	GetWorld()->GetTimerManager().SetTimer(Update2, this, &AFlockManager::random, 6.0f, true);



}

void AFlockManager::ImportData()
{

	//std::ifstream file("TrainingSet2.csv");
	//UE_LOG(LogTemp, Warning, TEXT("dataset is loaded"));
	//for (int row = 0; row < 14; ++row)
	//{
	//	std::string line;
	//	std::getline(file, line);
	//	if (!file.good())
	//		UE_LOG(LogTemp, Warning, TEXT("it broke away"));
	//		break;

	//	std::stringstream iss(line);
	//	UE_LOG(LogTemp, Warning, TEXT("in the middle of the loop"));
	//	for (int col = 0; col < 8; ++col)
	//	{
	//		std::string val;
	//		std::getline(iss, val, ',');
	//		if (!iss.good())
	//			break;

	//		std::stringstream convertor(val);
	//		convertor >> TrainingSet[row][col];
	//	}
	//}
	//UE_LOG(LogTemp, Warning, TEXT("TrainingSet index is :%d"),sizeof(TrainingSet));

}

void AFlockManager::StartData()
{

	float error = 1;
	int c = 0;
	while ((error > 0.05) && (c < 50000))
	{
		error = 0;
		c++;
		for (int i{ 0 }; i < 3; i++)
		{
			NeuralNetwork->SetInput(0, TrainingSet[i][0]);
			NeuralNetwork->SetInput(0, TrainingSet[i][1]);
			NeuralNetwork->SetInput(0, TrainingSet[i][2]);
			NeuralNetwork->SetInput(0, TrainingSet[i][3]);
			NeuralNetwork->SetInput(0, TrainingSet[i][4]);
			NeuralNetwork->SetInput(0, TrainingSet[i][5]);
			NeuralNetwork->SetInput(0, TrainingSet[i][6]);

			NeuralNetwork->SetDesiredOutput(0, TrainingSet[i][7]);
			NeuralNetwork->SetDesiredOutput(0, TrainingSet[i][8]);
			NeuralNetwork->SetDesiredOutput(0, TrainingSet[i][9]);
			NeuralNetwork->SetDesiredOutput(0, TrainingSet[i][10]);

			NeuralNetwork->FeedForward();
			error += NeuralNetwork->CalculateError();
			NeuralNetwork->BackPropogate();

		}
		error = error / 11.0f;
		UE_LOG(LogTemp, Warning, TEXT("total error is :%f"), error);
	}

}

void AFlockManager::Retrain(float evade, float chase, float merge, float DDay)
{

	float input0{ 0 };
	float input1{ 0 };
	float input2{ 0 };
	float input3{ 0 };
	float input4{ 0 };
	float input5{ 0 };
	float input6{ 0 };
	double error = 1;
	int c = 0;
	while ((error > 0.1) && (c < 5000))
	{
		for (AActor* Birdie : AllBirds)
		{
			if (IsValid(Birdie))
			{
				ABird* C = Cast<ABird>(Birdie);
				switch (C->MergeNum)
				{
				case 0:
					input0 = 1;
					input1 = 0;
					input2 = 0;
					input3 = 0;
					break;
				case 1:
					input0 = 0;
					input1 = 1;
					input2 = 0;
					input3 = 0;
					break;
				case 2:
					input0 = 0;
					input1 = 0;
					input2 = 1;
					input3 = 0;
					break;
				case 3:
					input0 = 0;
					input1 = 0;
					input2 = 0;
					input3 = 1;
					break;

				}
				input4 = C->HealthRef / C->HealthDefRef;
				UE_LOG(LogTemp, Warning, TEXT("input4 is :%f"), input4);
				input5 = StaticCast<float>(C->bcanDamage);
				UE_LOG(LogTemp, Warning, TEXT("input5 is :%f"), input5);
				input6 = StaticCast<float>(IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ABaseItem::StaticClass())));
				UE_LOG(LogTemp, Warning, TEXT("input6 is :%f"), input6);

				NeuralNetwork->SetInput(0, input0);
				NeuralNetwork->SetInput(1, input1);
				NeuralNetwork->SetInput(2, input2);
				NeuralNetwork->SetInput(3, input3);
				NeuralNetwork->SetInput(4, input4);
				NeuralNetwork->SetInput(5, input5);
				NeuralNetwork->SetInput(6, input6);
				NeuralNetwork->SetDesiredOutput(0, evade);
				NeuralNetwork->SetDesiredOutput(1, chase);
				NeuralNetwork->SetDesiredOutput(2, merge);
				NeuralNetwork->SetDesiredOutput(3, DDay);

				NeuralNetwork->FeedForward();
				error = NeuralNetwork->CalculateError();
				NeuralNetwork->BackPropogate();

			}
		}
	}


}

void AFlockManager::random()
{
	//this is the rule system we wanna train it to do
	float ref1 = StaticCast<float>(Mainchar->getHealth());
	for (AActor* Birdie : AllBirds)
	{
		if (IsValid(Birdie))
		{
			ABird* C = Cast<ABird>(Birdie);
			float ref = C->HealthRef / C->HealthDefRef;
			float ref2 = StaticCast<float>(IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ABaseItem::StaticClass())));
			if (ref1 < 0.5 && ref >= 0.5 && ref2 == 0.0f)
			{
				Retrain(0.1, 0.9, 0.1, 0.1);
			}
			else if (ref1 > 0.5 && ref > 0.5 && C->MergeNum < 4)
			{
				Retrain(0.1, 0.1, 0.9, 0.1);
			}
			else if (ref1 < 0.5 && ref2 == 1.0f)
			{
				Retrain(0.1, 0.1, 0.1, 0.9);
			}
			else if (ref1 > 0.5 && ref <= 0.5 && ref2 == 0.0f)
			{
				Retrain(0.9, 0.1, 0.1, 0.1);
			}

			//need to retrain it to merge
		}
	}

}


void AFlockManager::newCommand()
{
	const float epsilon = 0.2; //random variable
	float randomgen = FMath::RandRange(0.0, 1.0);
	if (randomgen < epsilon)
	{
		Temp();//generates random state
		UE_LOG(LogTemp, Warning, TEXT("temp"));
	}
	else
	{
		float input0{ 0 };
		float input1{ 0 };
		float input2{ 0 };
		float input3{ 0 };
		float input4{ 0 };
		float input5{ 0 };
		float input6{ 0 };
		for (AActor* Birdie : AllBirds)
		{
			if (IsValid(Birdie))
			{
				ABird* C = Cast<ABird>(Birdie);
				switch (C->MergeNum)
				{
				case 0:
					input0 = 1;
					input1 = 0;
					input2 = 0;
					input3 = 0;
					break;
				case 1:
					input0 = 0;
					input1 = 1;
					input2 = 0;
					input3 = 0;
					break;
				case 2:
					input0 = 0;
					input1 = 0;
					input2 = 1;
					input3 = 0;
					break;
				case 3:
					input0 = 0;
					input1 = 0;
					input2 = 0;
					input3 = 1;
					break;

				}
				input4 = C->HealthRef / C->HealthDefRef;
				input5 = StaticCast<float>(C->bcanDamage);
				input6 = StaticCast<float>(IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ABaseItem::StaticClass())));

				NeuralNetwork->SetInput(0, input0);
				NeuralNetwork->SetInput(1, input1);
				NeuralNetwork->SetInput(2, input2);
				NeuralNetwork->SetInput(3, input3);
				NeuralNetwork->SetInput(4, input4);
				NeuralNetwork->SetInput(5, input5);
				NeuralNetwork->SetInput(6, input6);
				NeuralNetwork->FeedForward();
			}
		}
		pn = NeuralNetwork->GetMaxOutputID();
		UE_LOG(LogTemp, Warning, TEXT("command is :%d"), pn);
	}

}

// Called every frame
void AFlockManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlockManager::StateUpdate()
{
	//UE_LOG(LogTemp, Warning, TEXT("updating"));
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
	else if (CurrentState == BirdState::DESTROY)
	{

		if (CurrentAction == BirdAction::ON_ENTER)
		{
			Destroy_Enter();
		}
		if (CurrentAction == BirdAction::ON_UPDATE)
		{
			Destroy_Update();
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
	case BirdState::DESTROY:
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
	if (pn == 3)
	{
		SetBossState(BirdState::DESTROY);
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
	if (pn == 3)
	{
		SetBossState(BirdState::DESTROY);
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
	if (pn == 3)
	{
		SetBossState(BirdState::DESTROY);
	}
}

void AFlockManager::Destroy_Enter()
{
	CurrentAction = BirdAction::ON_UPDATE;
}

void AFlockManager::Destroy_Update()
{
	DestroyPhase();
	if (pn != 3)
	{
		Destroy_Exit();
	}
}

void AFlockManager::Destroy_Exit()
{
	if (pn == 0)
	{
		SetBossState(BirdState::IDLE);
	}
	if (pn == 1)
	{
		SetBossState(BirdState::CHASE);
	}
	if (pn == 2)
	{
		SetBossState(BirdState::MERGE);
	}
}
