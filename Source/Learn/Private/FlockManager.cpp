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
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"



// Sets default values
AFlockManager::AFlockManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	MaxBirds = 10;
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


	Mainchar = Cast<AMCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMCharacter::StaticClass()));
	NeuralNetwork = Cast<AFNN>(UGameplayStatics::GetActorOfClass(GetWorld(), AFNN::StaticClass()));

	if (IsValid(NeuralNetwork))
	{
		NeuralNetwork->Initialize(7, 6, 4);
		NeuralNetwork->SetLearningRate(0.2);
		NeuralNetwork->SetMomentum(true, 0.9);
		UE_LOG(LogTemp, Warning, TEXT("TrainSet index is :%f"), TrainingSet[2][1]);
	}
	else
	{
		NeuralNetwork = Cast<AFNN>(UGameplayStatics::GetActorOfClass(GetWorld(), AFNN::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("network is not set"));
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

	pn = 0;

	ImportData(); //import dataset

	Initialize();
	if (NeuralNetwork != nullptr)
	{
		StartData();

		FTimerHandle Testing;
		GetWorld()->GetTimerManager().SetTimer(Testing, this, &AFlockManager::newCommand, 5.0f, true);

		FTimerHandle Update;
		GetWorld()->GetTimerManager().SetTimer(Update, this, &AFlockManager::StateUpdate, 1.0f, true);

		FTimerHandle Update2;
		GetWorld()->GetTimerManager().SetTimer(Update2, this, &AFlockManager::random, 6.0f, true);
	}




}

void AFlockManager::ImportData()
{

	//Acquire the fpath for the training data in the data folder in the content
	FString fPath = FPaths::ProjectContentDir() + TEXT("Data/TrainingSet2.csv");
	UE_LOG(LogTemp, Warning, TEXT("FilePaths: ProjectContentDir: %s"), *fPath);

	TestSet; //training data array not used yet
	TArray<TArray<FString>> parsedCSV;


	FString MyFilePath = FPaths::ProjectContentDir(); //same thing as above delete later
	MyFilePath.Append(TEXT("Data/TrainingSet2.csv")); //change this out to whatever csv file you are using for the data. 
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (FileManager.FileExists(*MyFilePath)) //check if the file exists
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: File found!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePaths: File not found!"));
	}

	if (FPaths::FileExists(fPath))
	{
		FString FileContent;
		//Read the csv file
		FFileHelper::LoadFileToString(FileContent, *fPath);
		UE_LOG(LogTemp, Error, TEXT("%s"), *FileContent);

		const TCHAR* Terminators[] = { L"\r", L"\n" }; //LINE_TERMINATOR
		const TCHAR* CSVDelimeters[] = { TEXT(",") };

		TArray<FString> CSVLines;
		FileContent.ParseIntoArray(CSVLines, Terminators, 2);

		TArray<FString> temp_array;
		for (int i = 0; i < CSVLines.Num(); i++) {
			temp_array.Empty();
			CSVLines[i].ParseIntoArray(temp_array, CSVDelimeters, 1);
			parsedCSV.Add(temp_array);
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("no path found"));
	}

	for (int i = 0; i < parsedCSV.Num(); i++)
	{
		TArray<float>temp;
		temp.Init(0, parsedCSV.Num());
		for (int j = 0; j < parsedCSV[i].Num(); j++)
		{
			//UE_LOG(LogTemp, Error, TEXT("%n"),j);
			float show = FCString::Atof(*parsedCSV[i][j]);
			temp.Add(show);
			UE_LOG(LogTemp, Error, TEXT("%f"), show);
		}
		TestSet.Add(temp);
		temp.Empty();
	}

	for (int i = 0; i < TestSet.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT(" testset.num: %d"), TestSet[i].Num());
	}

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
			NeuralNetwork->SetInput(0, TestSet[i][0]);
			NeuralNetwork->SetInput(0, TestSet[i][1]);
			NeuralNetwork->SetInput(0, TestSet[i][2]);
			NeuralNetwork->SetInput(0, TestSet[i][3]);
			NeuralNetwork->SetInput(0, TestSet[i][4]);
			NeuralNetwork->SetInput(0, TestSet[i][5]);
			NeuralNetwork->SetInput(0, TestSet[i][6]);

			NeuralNetwork->SetDesiredOutput(0, TestSet[i][7]);
			NeuralNetwork->SetDesiredOutput(0, TestSet[i][8]);
			NeuralNetwork->SetDesiredOutput(0, TestSet[i][9]);
			NeuralNetwork->SetDesiredOutput(0, TestSet[i][10]);

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
				//UE_LOG(LogTemp, Warning, TEXT("input4 is :%f"), input4);
				input5 = StaticCast<float>(AllBirds.Num()/10);
				//UE_LOG(LogTemp, Warning, TEXT("input5 is :%f"), input5);
				input6 = StaticCast<float>(IsValid(UGameplayStatics::GetActorOfClass(GetWorld(), ABaseItem::StaticClass())));
				//UE_LOG(LogTemp, Warning, TEXT("input6 is :%f"), input6);

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
				ABirdController* R = Cast<ABirdController>(C->GetController());
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
				input5 = StaticCast<float>((R->Manager->AllBirds.Num())/10);
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
			pn = NeuralNetwork->GetMaxOutputID(); // move this up after the feedforward
		}

		//UE_LOG(LogTemp, Warning, TEXT("command is :%d"), pn);
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
