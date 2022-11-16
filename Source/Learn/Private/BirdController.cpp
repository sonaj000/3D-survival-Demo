// Fill out your copyright notice in the Description page of Project Settings.


#include "BirdController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Bird.h"
#include "AI/NavigationSystemBase.h"
#include "AI/Navigation/NavigationTypes.h"
#include"BigBird.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "FlockManager.h"

ABirdController::ABirdController(const FObjectInitializer& ObjectInitializer)
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp")));

	SightRadius = 1000;
	FieldofView = 360.0f;
	SightAge = 3.0f;
	LoseSight = SightRadius + 5.0f;

	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSight; //max distance in which bird starts to lose sight
	SightConfig->PeripheralVisionAngleDegrees = FieldofView;//direction of vision 
	SightConfig->SetMaxAge(SightAge); // how long the sight is remembered

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation()); //if we utilize more than one sense 
	//GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ABirdController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	bisCharacterDetected = false;
	DistanceToPlayer = 0.0;

	bisChasing = false;
	bisEvading = false;
	bisFlocking = false;
	flocknumber = 0;

	ChaseMat = CreateDefaultSubobject<UMaterial>(TEXT("Chase Materia"));
	EvadeMat = CreateDefaultSubobject<UMaterial>(TEXT("Evade Material"));
	FlockMat = CreateDefaultSubobject<UMaterial>(TEXT("Flock Material"));


}

void ABirdController::BeginPlay()
{
	Super::BeginPlay();


	//FTimerHandle Testing;
	//GetWorld()->GetTimerManager().SetTimer(Testing, this, &ABirdController::Flocking, 1.0f,true); 

	testarray.AddUnique(this->GetPawn());

	Manager = Cast<AFlockManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFlockManager::StaticClass()));

	UE_LOG(LogTemp, Warning, TEXT("managert name is :%s"), *Manager->GetName())
}

void ABirdController::OnPossess(APawn* cPawn)
{
	Super::OnPossess(cPawn);
}

void ABirdController::Chasing()
{
	StateChange(1);
	//need to change movement speed in state change
	try
	{
		UE_LOG(LogTemp, Warning, TEXT("is greater than 2000"));
		//this->MoveToActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));//swap this out for actor location
		this->MoveToLocation(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
	}
	catch (const std::exception&)
	{
		UE_LOG(LogTemp, Warning, TEXT("no work"));
	}
	

}

void ABirdController::Evading()
{
	/* select a random location on the navmesh, double check it's distance from the player is greater than x distance and if not repick a location*/
	StateChange(2);
	UE_LOG(LogTemp, Warning, TEXT("Evading"));
	UNavigationSystemV1* NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FVector RandomLoc = FVector(0, 0, 0);
	FNavLocation ResultLocation;
	bool bSuccess = NavArea->GetRandomReachablePointInRadius(NavArea->GetWorldBounds().GetCenter(), 5000.0f, ResultLocation);
	if (bSuccess)
	{
		RandomLoc = ResultLocation;
	}
	///FVector RandomLoc = NavArea->GetRandomReachablePointInRadius(this, NavArea->GetWorldBounds().GetCenter(), 2000.0f); // change the radius of the randomization
	FVector PlayerLoc = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	/// <summary>
	/// maybe make this a while loop instead of if. 
	/// </summary>
	if (FVector::Distance(RandomLoc, PlayerLoc) > 4000.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("is greater than 2000"));
		this->MoveToLocation(RandomLoc);
	}
	else
	{
		Evading();
	}
}

void ABirdController::Flocking()
{
	StateChange(3);
	if (testarray.Num() > 1 && (Cast<ABird>(GetPawn())->MergeNum) < 4) //change this to include all birds later. 
	{
		//Manager->MergeFlock(testarray);
		Manager->CheckUnique(testarray);
		//Manager->tf();
		UE_LOG(LogTemp, Warning, TEXT("tf function called: array number is :%d"), testarray.Num());
	}
	else
	{
		UNavigationSystemV1* NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		FVector RandomLoc = FVector(0, 0, 0);
		FNavLocation ResultLocation;
		bool bSuccess = NavArea->GetRandomReachablePointInRadius(NavArea->GetWorldBounds().GetCenter(), 5000.0f, ResultLocation);
		if (bSuccess)
		{
			RandomLoc = ResultLocation;
		}
		///FVector RandomLoc = NavArea->GetRandomReachablePointInRadius(this, NavArea->GetWorldBounds().GetCenter(), 2000.0f); // change the radius of the randomization
		FVector PlayerLoc = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
		/// <summary>
		/// maybe make this a while loop instead of if. 
		/// </summary>
		if (FVector::Distance(RandomLoc, PlayerLoc) > 4000.0f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("is greater than 2000"));
			this->MoveToLocation(RandomLoc);
		}
	}


	/* the flocking logic should be managed by a flock manager. we can just have the boxes either stack on top of each other */
	//import a flockmanager. 
	/*flock manager should keep track of all the birds on the level via get all actors of class function to store them all in a array
	boolean is flocking is true. 
	
	
	//logic for the flock manager who will handle the flocking unit. 
	use oncomponentoverlap. so have a secondary sphere that's not hitbox and basically if there is an actor overlapping that is a bird, add it to a map so use umap on the flockmanager. 
	there will be a group for each bird that is stored in the flock so unit from array index is key and value is group number. 

	so first check for each actor in overlap that is a bird if there is an input in the map for the bird name. 
	if (yes)
	return
	else:
	proceed with making an array in the flockmanager and add the bird and all other bird actors overlapping into the array. add each bird to the map and their corresponding group. 


	for now, let's just make an array for each group and add the bird in there. so basically we randomly select a bird from the array and then have all the birds in that group flock towards the llocation of that actor. So 
	CHANGE THE FUNCTION ARGUMENT FROM VOID TO FVECTOR. 

	flock manager should have one tarray that has all the birds, one map that gets filled for each group and bird in the group, and i lied we can store the array in the map. maybe to make it more balanced, set the flock speed slower?

	so one thing we can do is replace all the birds with simply one bird with a bigger mesh and just name this big bird lol. would be easier and we would not have to worru about coding things like having all the birds stick together and stay togetehr which is like weird
	so one thing is all these birds need to have destructors to destroy at the end. 
	
	*/

}

void ABirdController::StateChange(int statenum)
{ // changing the boolean but also the mesh of the box to indicae which state it is in 

	if (GetPawn() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("fuck"));
		return;

	}
	else
	{

		AActor* H = Cast<AActor>(this->GetPawn());
		UStaticMeshComponent* T = Cast<UStaticMeshComponent>(H->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		switch (statenum)
		{
		case 1:
			bisChasing = true;
			bisEvading = false;
			bisFlocking = false;
			T->SetMaterial(0, ChaseMat); // set material here. might make this a seperate function
			break;

		case 2:
			bisChasing = false;
			bisEvading = true;
			bisFlocking = false;
			T->SetMaterial(0, EvadeMat);
			break;

		case 3:
			bisChasing = false;
			bisEvading = false;
			bisFlocking = true;
			T->SetMaterial(0, FlockMat);
			break;
		}
	}

}


void ABirdController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}