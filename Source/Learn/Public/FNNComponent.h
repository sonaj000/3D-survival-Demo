// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FNNComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEARN_API UFNNComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFNNComponent();

	UPROPERTY(VisibleAnywhere, Category = "Setup")
		int NumberofNodes;
	UPROPERTY(VisibleAnywhere, Category = "Setup")
		int NumberofChildNodes;
	UPROPERTY(VisibleAnywhere, Category = "Setup")
		int NumberofParentNodes;


	TArray<TArray<float>>TestWeights;


	TArray<TArray<float>>TestWeightsChanges;


	TArray<float>TestNeuronValues;


	TArray<float>TestDesiredValues;


	TArray<float>TestErrors;


	TArray<float>TestBiasWeights;


	TArray<float>TestBiasValues;


	UPROPERTY(VisibleAnywhere, Category = "Setup")
		double LearningRate;

	UPROPERTY(VisibleAnywhere, Category = "Setup")
		bool LinearOutput;
	UPROPERTY(VisibleAnywhere, Category = "Setup")
		bool UseMomentum;
	UPROPERTY(VisibleAnywhere, Category = "Setup")
		double MomentumFactor;

	UPROPERTY(VisibleAnywhere, Category = "Layers")
		UFNNComponent* ParentLayer;

	UPROPERTY(VisibleAnywhere, Category = "Layers")
		UFNNComponent* ChildLayer;

	UFUNCTION()
		void Initialize(int NumNodes, UFNNComponent* parent, UFNNComponent* child);

	UFUNCTION()
		void CleanUp();

	UFUNCTION()
		void RandomizeWeights();///this is the same thing as void for the argument which mans takes nothing

	UFUNCTION()
		void CalculateErrors();

	UFUNCTION()
		void AdjustWeights();

	UFUNCTION()
		void CalculateNeuronValues();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
