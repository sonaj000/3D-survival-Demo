// Fill out your copyright notice in the Description page of Project Settings.


#include "FNN.h"
#include "FNNComponent.h"

// Sets default values
AFNN::AFNN()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFNN::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFNN::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFNN::Initialize(int nNodesInput, int nNodesHidden, int nNodesOutput)
{
	InputLayer->NumberofNodes = nNodesInput;
	InputLayer->NumberofChildNodes = nNodesHidden;
	InputLayer->NumberofParentNodes = 0;
	InputLayer->Initialize(nNodesInput, NULL, HiddenLayer);
	InputLayer->RandomizeWeights();

	HiddenLayer->NumberofNodes = nNodesHidden;
	HiddenLayer->NumberofChildNodes = nNodesOutput;
	HiddenLayer->NumberofParentNodes = nNodesInput;
	HiddenLayer->Initialize(nNodesHidden, InputLayer, OutputLayer);
	HiddenLayer->RandomizeWeights();

	OutputLayer->NumberofNodes = nNodesOutput;
	OutputLayer->NumberofChildNodes = 0;
	OutputLayer->NumberofParentNodes = nNodesHidden;
	OutputLayer->Initialize(nNodesOutput, HiddenLayer, NULL);

}

void AFNN::CleanUp()
{
	InputLayer->CleanUp();
	HiddenLayer->CleanUp();
	OutputLayer->CleanUp();
}

void AFNN::SetInput(int i, double value)
{
	if ((i >= 0) && (i < InputLayer->NumberofNodes))
	{
		InputLayer->NeuronValues[i] = value;
	}
}

double AFNN::GetOutput(int i)
{
	if ((i >= 0) && (i < OutputLayer->NumberofNodes))
	{
		return OutputLayer->NeuronValues[i];
	}

	return (double)INT_MAX;
}

void AFNN::SetDesiredOutput(int i, double value)
{
	if ((i >= 0) && (i < OutputLayer->NumberofNodes))
	{
		OutputLayer->DesiredValues[i] = value;
	}
}

void AFNN::FeedForward()
{
	InputLayer->CalculateNeuronValues();
	HiddenLayer->CalculateNeuronValues();
	OutputLayer->CalculateNeuronValues();
}

void AFNN::BackPropogate()
{
	OutputLayer->CalculateErrors();
	HiddenLayer->CalculateErrors();

	HiddenLayer->AdjustWeights();
	InputLayer->AdjustWeights();

}

int AFNN::GetMaxOutputID()
{
	int i, id;
	double maxval;

	maxval = OutputLayer->NeuronValues[0];
	id = 0;

	for (i = 1; i < OutputLayer->NumberofNodes; i++)
	{
		if (OutputLayer->NeuronValues[i] > maxval)
		{
			maxval = OutputLayer->NeuronValues[i];
			id = i;
		}
	}
	return id;
}

double AFNN::CalculateError()
{
	int i;
	double error = 0;

	for (i = 0; i, OutputLayer->NumberofNodes; i++)
	{
		error += pow(OutputLayer->NeuronValues[i] - OutputLayer->DesiredValues[i], 2);

	}

	error = error / OutputLayer->NumberofNodes;

	return error;

}

void AFNN::SetLearningRate(double rate)
{
	InputLayer->LearningRate = rate;
	HiddenLayer->LearningRate = rate;
	OutputLayer->LearningRate = rate;
}

void AFNN::SetLinearOutput(bool useLinear)
{
	InputLayer->LinearOutput = useLinear;
	HiddenLayer->LinearOutput = useLinear;
	OutputLayer->LinearOutput = useLinear;
}

void AFNN::SetMomentum(bool useMomentum, double factor)
{
	InputLayer->UseMomentum = useMomentum;
	HiddenLayer->UseMomentum = useMomentum;
	OutputLayer->UseMomentum = useMomentum;

	InputLayer->MomentumFactor = factor;
	HiddenLayer->MomentumFactor = factor;
	OutputLayer->MomentumFactor = factor;
}



