// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bird.h"
#include "FlockManager.generated.h"


class ABird;
class ABigBird;

UENUM()
enum BirdState { IDLE,EVADE, CHASE, MERGE };
UENUM()
enum BirdAction { ON_ENTER, ON_UPDATE };

UCLASS()
class LEARN_API AFlockManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockManager();

	TArray<AActor*>AllBirds;

	UPROPERTY(VisibleAnywhere, Category = "Merge")
	TSubclassOf<ABigBird>NewBB;


	UPROPERTY(EditDefaultsOnly, Category = "bigbird")
		TSubclassOf<ABigBird>BB;

	UFUNCTION()
		void Initialize();

	UFUNCTION()
		void MergeFlock(TArray<AActor*>NewFlock);

	UFUNCTION()
		void DestroyFlock(TArray<AActor*>NewFlock, FVector spawnloc);


		void CheckUnique(TArray<AActor*>RF);

	UFUNCTION()
		void tf(); 

	UFUNCTION()
		void chasePhase();

	UFUNCTION()
		void evadePhase();
	UFUNCTION()
		void Temp();

	UPROPERTY()
		int pn;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "bigbird")
		TSubclassOf<ABird> MyActorClass = ABird::StaticClass();

	TArray<TArray<AActor*>>Cool;

	UPROPERTY()
		TMap<AActor*, bool> GroupChecker;

private:
	UPROPERTY()
		TEnumAsByte<BirdState>CurrentState = BirdState::IDLE;;

	UPROPERTY()
		TEnumAsByte<BirdAction>CurrentAction = BirdAction::ON_ENTER;;

	UFUNCTION()
		void StateUpdate();

	UFUNCTION()
		void SetBossState(BirdState newState);

	void Idle_Enter();
	void Idle_Update();
	void Idle_Exit();

	void Chase_Enter();
	void Chase_Update();
	void Chase_Exit();

	void Merge_Enter();
	void Merge_Update();
	void Merge_Exit();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
