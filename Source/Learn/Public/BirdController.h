// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BirdController.generated.h"

/**
 * 
 */

class UBoxComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class LEARN_API ABirdController : public AAIController
{
	GENERATED_BODY()

public:

	ABirdController(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* thePawn) override;

	UPROPERTY()
		bool bisChasing;

	UPROPERTY()
		bool bisEvading;

	UPROPERTY()
		bool bisFlocking;

	UPROPERTY()
		int flocknumber;

	UFUNCTION()
		void Chasing();

	UFUNCTION()
		void Evading();

	UFUNCTION()
		void Flocking();

	UFUNCTION()
		void StateChange(int statenum);

	UPROPERTY()
		FVector CurrLoc;

protected:
	///ai senses here
	UFUNCTION(BlueprintCallable)
		void OnPawnDetected(const TArray<AActor*>& DetectedPawns);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
		float SightRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
		float SightAge;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
		float LoseSight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
		float FieldofView;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		bool bisCharacterDetected;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		float DistanceToPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		AActor* Holder;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "materials")
		UMaterial* ChaseMat;

	UPROPERTY(EditAnywhere, Category = "materials")
		UMaterial* EvadeMat;

	UPROPERTY(EditAnywhere, Category = "materials")
		UMaterial* FlockMat;








	
};
