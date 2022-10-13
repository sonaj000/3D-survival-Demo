// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FNN.generated.h"

class UFNNComponent;
UCLASS()
class LEARN_API AFNN : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFNN();

	UPROPERTY(VisibleAnywhere, Category = "Layers")
		UFNNComponent* InputLayer;

	UPROPERTY(VisibleAnywhere, Category = "Layers")
		UFNNComponent* HiddenLayer;

	UPROPERTY(VisibleAnywhere, Category = "Layers")
		UFNNComponent* OutputLayer;

	UFUNCTION()
		void Initialize(int nNodesInput, int nNodesHidden, int nNodesOutput);
	UFUNCTION()
		void CleanUp();
	UFUNCTION()
	void SetInput(int i, double value);
	UFUNCTION()
		double GetOutput(int i);
	UFUNCTION()
		void SetDesiredOutput(int i, double value);
	UFUNCTION()
		void FeedForward();
	UFUNCTION()
		void BackPropogate();
	UFUNCTION()
		int GetMaxOutputID();
	UFUNCTION()
		double CalculateError();
	UFUNCTION()
		void SetLearningRate(double rate);
	UFUNCTION()
		void SetLinearOutput(bool useLinear);
	UFUNCTION()
		void SetMomentum(bool useMomentum, double factor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
