// Fill out your copyright notice in the Description page of Project Settings.


#include "FNNComponent.h"

// Sets default values for this component's properties
UFNNComponent::UFNNComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.bCanEverTick = true;
	ParentLayer = NULL;
	ChildLayer = NULL;
	LinearOutput = false;
	UseMomentum = false;
	MomentumFactor = 0.9;

	// ...
}
void UFNNComponent::Initialize(int NumNodes, UFNNComponent* parent, UFNNComponent* child)
{

	if (parent != NULL)
	{
		ParentLayer = parent;
	}
	if (child != NULL)
	{
		ChildLayer = child;

	}

	TestNeuronValues.Init(0, NumberofNodes);
	TestDesiredValues.Init(0, NumberofNodes);
	TestErrors.Init(0, NumberofNodes);

	if (ChildLayer != NULL)
	{
		TArray<float>co;
		co.Init(0, NumberofNodes);

		UE_LOG(LogTemp, Warning, TEXT("size of ineer most weights: %d"), co.Num());
		TestWeights.Init(co, NumberofNodes);

		UE_LOG(LogTemp, Warning, TEXT("size of test weights: %d"), TestWeights.Num());
		TestWeightsChanges.Init(co, NumberofNodes);

		TestBiasValues.Init(-1, NumberofChildNodes);
		TestBiasWeights.Init(0, NumberofChildNodes);
	}

}

void UFNNComponent::CleanUp(void)
{
	//free(NeuronValues);
	//free(DesiredValues);
	//free(Errors);

	//if (Weights != NULL)
	//{
	//	for (int i{ 0 }; i < NumberofNodes; i++)
	//	{
	//		free(Weights[i]);
	//		free(WeightChanges[i]);
	//	}

	//	free(Weights);
	//	free(WeightChanges);

	//}
	//if (BiasValues != NULL) free(BiasValues);
	//if (BiasWeights != NULL) free(BiasWeights);

}

void UFNNComponent::RandomizeWeights()
{
	int min{ 0 };
	int max = 200;
	int number;
	srand((unsigned)time(NULL));
	for (int i{ 0 }; i < NumberofNodes; i++)
	{
		for (int j{ 0 }; j < NumberofChildNodes; j++)
		{
			number = (((abs(rand()) % (max - min + 1)) + min));

			if (number > max)
			{
				number = max;
			}
			if (number < min)
			{
				number = min;
			}
			TestWeights[i][j] = number / 100.0f - 1;
		}
	}

	for (int j{ 0 }; j < NumberofChildNodes; j++)
	{
		number = (((abs(rand()) % (max - min + 1)) + min));

		if (number > max)
		{
			number = max;
		}
		if (number < min)
		{
			number = min;
		}

		TestBiasWeights[j] = number / 100.0f - 1;
	}

}

void UFNNComponent::CalculateErrors()
{
	double sum;
	if (ChildLayer == NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			TestErrors[i] = (TestDesiredValues[i] - TestNeuronValues[i]) * TestNeuronValues[i] * (1.0f - TestNeuronValues[i]);
		}
	}
	else if (ParentLayer == NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			TestErrors[i] = 0.0f;
		}
	}
	else
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			sum = 0;
			for (int j{ 0 }; j < NumberofChildNodes; j++)
			{
				sum += ChildLayer->TestErrors[j] * TestWeights[i][j];
			}
			TestErrors[i] = sum * TestNeuronValues[i] * (1.0f - TestNeuronValues[i]);
		}
	}

}

void UFNNComponent::AdjustWeights()
{
	double dw;

	if (ChildLayer != NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			for (int j{ 0 }; j < NumberofChildNodes; j++)
			{
				dw = LearningRate * ChildLayer->TestErrors[j] * TestNeuronValues[i];
				if (UseMomentum)
				{
					TestWeights[i][j] += dw + MomentumFactor * TestWeightsChanges[i][j];
					TestWeightsChanges[i][j] += dw;
				}
				else
				{
					TestWeights[i][j] += dw;
				}
			}
		}
		for (int j{ 0 }; j < NumberofChildNodes; j++)
		{
			TestBiasWeights[j] += LearningRate * ChildLayer->TestErrors[j] * TestBiasValues[j];
		}
	}

}

void UFNNComponent::CalculateNeuronValues()
{
	double x;
	if (ParentLayer != NULL)
	{
		for (int j{ 0 }; j < NumberofNodes; j++)
		{
			x = 0;
			for (int i{ 0 }; i < NumberofParentNodes; i++)
			{
				x += ParentLayer->TestNeuronValues[i] * ParentLayer->TestWeights[i][j];
			}
			x += ParentLayer->TestBiasValues[j] * ParentLayer->TestBiasWeights[j];

			if ((ChildLayer == NULL) && LinearOutput)
			{
				TestNeuronValues[j] = x;
			}
			else
			{
				TestNeuronValues[j] = 1.0f / (1 + exp(-x));
			}
		}
	}
}

// Called when the game starts
void UFNNComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UFNNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

