// Fill out your copyright notice in the Description page of Project Settings.


#include "FNN.h"
#include "FNNComponent.h"

// Sets default values
AFNN::AFNN()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	InputLayer = CreateDefaultSubobject<UFNNComponent>(TEXT("Input Layer"));
	HiddenLayer = CreateDefaultSubobject<UFNNComponent>(TEXT("Hidden Layer"));
	OutputLayer = CreateDefaultSubobject<UFNNComponent>(TEXT("Output Layer"));

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
	UE_LOG(LogTemp, Warning, TEXT("hidden layer test weights:  %d"), HiddenLayer->TestWeights.Num());
	UE_LOG(LogTemp, Warning, TEXT("here is weights index test : %f"), HiddenLayer->TestWeights[1][0]);
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
		InputLayer->TestNeuronValues[i] = value;
	}
}

double AFNN::GetOutput(int i)
{
	if ((i >= 0) && (i < OutputLayer->NumberofNodes))
	{
		return OutputLayer->TestNeuronValues[i];
	}

	return (double)INT_MAX;
}

void AFNN::SetDesiredOutput(int i, double value)
{
	if ((i >= 0) && (i < OutputLayer->NumberofNodes))
	{
		OutputLayer->TestDesiredValues[i] = value;
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

	maxval = OutputLayer->TestNeuronValues[0];
	id = 0;

	for (i = 1; i < OutputLayer->NumberofNodes; i++)
	{
		if (OutputLayer->TestNeuronValues[i] > maxval)
		{
			maxval = OutputLayer->TestNeuronValues[i];
			id = i;
		}
	}
	return id;
}

double AFNN::CalculateError()
{
	int i;
	double error = 0;

	for (i = 0; i < OutputLayer->NumberofNodes; i++)
	{
		error += pow(OutputLayer->TestNeuronValues[i] - OutputLayer->TestDesiredValues[i], 2);

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


