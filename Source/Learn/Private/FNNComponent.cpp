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
	NeuronValues = (double*)malloc(sizeof(double) * NumberofNodes);
	DesiredValues = (double*)malloc(sizeof(double) * NumberofNodes);

	Errors = (double*)malloc(sizeof(double) * NumberofNodes);

	if (parent != NULL)
	{
		if (child != NULL)
		{
			ChildLayer = child;

			Weights = (double**)malloc(sizeof(double*) * NumberofNodes);

			WeightChanges = (double**)malloc(sizeof(double*) * NumberofNodes);

			for (int i{ 0 }; i < NumberofNodes; i++)
			{
				Weights[i] = (double*)malloc(sizeof(double) * NumberofChildNodes);

				WeightChanges[i] = (double*)malloc(sizeof(double) * NumberofChildNodes);
			}

			BiasValues = (double*)malloc(sizeof(double) * NumberofChildNodes);

			BiasWeights = (double*)malloc(sizeof(double) * NumberofChildNodes);
		}
		else
		{
			Weights = NULL;
			BiasValues = NULL;
			BiasWeights = NULL;
			WeightChanges = NULL;
		}
		//initialzing zeros for everything
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			NeuronValues[i] = 0;
			DesiredValues[i] = 0;
			Errors[i] = 0;

			if (ChildLayer != NULL)
			{
				for (int j{ 0 }; j < NumberofNodes; j++)
				{
					Weights[i][j] = 0;
					WeightChanges[i][j] = 0;
				}
			}

		}
		//initialize the bias values and weights 
		if (ChildLayer != NULL)
		{
			for (int j{ 0 }; j < NumberofChildNodes; j++)
			{
				BiasValues[j] = -1;
				BiasWeights[j] = 0;

			}
		}
	}
}

void UFNNComponent::CleanUp(void)
{
	free(NeuronValues);
	free(DesiredValues);
	free(Errors);

	if (Weights != NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			free(Weights[i]);
			free(WeightChanges[i]);
		}

		free(Weights);
		free(WeightChanges);

	}
	if (BiasValues != NULL) free(BiasValues);
	if (BiasWeights != NULL) free(BiasWeights);

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
			Weights[i][j] = number / 100.0f - 1;
		}
	}

	for (int j{ 0 }; j < NumberofNodes; j++)
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

		BiasWeights[j] = number / 100.0f - 1;
	}

}

void UFNNComponent::CalculateErrors()
{
	double sum;
	if (ChildLayer == NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			Errors[i] = (DesiredValues[i] - NeuronValues[i]) * NeuronValues[i] * (1.0f - NeuronValues[i]);
		}
	}
	else if (ParentLayer == NULL)
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			Errors[i] = 0.0f;
		}
	}
	else
	{
		for (int i{ 0 }; i < NumberofNodes; i++)
		{
			sum = 0;
			for (int j{ 0 }; j < NumberofChildNodes; j++)
			{
				sum += ChildLayer->Errors[j] * Weights[i][j];
			}
			Errors[i] = sum * NeuronValues[i] * (1.0f - NeuronValues[i]);
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
			for (int j{ 0 }; j < NumberofNodes; j++)
			{
				dw = LearningRate * ChildLayer->Errors[j] * NeuronValues[i];
				if (UseMomentum)
				{
					Weights[i][j] += dw + MomentumFactor * WeightChanges[i][j];
					WeightChanges[i][j] += dw;
				}
				else
				{
					Weights[i][j] += dw;
				}
			}
		}
		for (int j{ 0 }; j < NumberofChildNodes; j++)
		{
			BiasWeights[j] += LearningRate * ChildLayer->Errors[j] * BiasValues[j];
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
				x += ParentLayer->NeuronValues[i] * ParentLayer->Weights[i][j];
			}
			x += ParentLayer->BiasValues[j] * ParentLayer->BiasWeights[j];

			if ((ChildLayer == NULL) && LinearOutput)
			{
				NeuronValues[j] = x;
			}
			else
			{
				NeuronValues[j] = 1.0f / (1 + exp(-x));
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

