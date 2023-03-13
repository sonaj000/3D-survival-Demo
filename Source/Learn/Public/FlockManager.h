// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bird.h"
#include "FlockManager.generated.h"


class ABird;
class ABigBird;
class AFNN;
class AMCharacter;

UENUM()
enum BirdState { IDLE, EVADE, CHASE, MERGE, DESTROY };
UENUM()
enum BirdAction { ON_ENTER, ON_UPDATE };

UCLASS()
class LEARN_API AFlockManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlockManager();

	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* StaticMesh;

	TArray<AActor*>AllBirds;

	UPROPERTY(VisibleAnywhere, Category = "Merge")
		TSubclassOf<ABigBird>NewBB;


	UPROPERTY(EditDefaultsOnly, Category = "bigbird")
		TSubclassOf<ABigBird>BB;

	UPROPERTY(EditAnywhere, Category = "Max Birds")
		int MaxBirds;

	UFUNCTION()
		void Initialize();

	UFUNCTION()
		void tf();

	UFUNCTION()
		void chasePhase();

	UFUNCTION()
		void evadePhase();
	UFUNCTION()
		void DestroyPhase();
	UFUNCTION()
		void Temp();

	UPROPERTY()
		int pn;

	UPROPERTY()
		TMap<AActor*, bool> GroupChecker;

	TArray<TArray<float>>TrainingSet;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "bigbird")
		TSubclassOf<ABird> MyActorClass = ABird::StaticClass();

	TArray<TArray<AActor*>>Cool;

	TArray<TArray<float>>TestSet;

	void ImportData();

	UFUNCTION()
		void StartData();

	UFUNCTION()
		void Retrain(float evade, float chase, float merge, float DDay);

	UFUNCTION()
		void random();

	UFUNCTION()
		void newCommand();

	UPROPERTY()
		AMCharacter* Mainchar;

	UPROPERTY(EditAnywhere, Category = "Brain")
		/*TSubclassOf<AFNN>NeuralNetwork; */
		AFNN* NeuralNetwork;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<AFNN>NeuralNet;

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

	void Destroy_Enter();
	void Destroy_Update();
	void Destroy_Exit();



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
