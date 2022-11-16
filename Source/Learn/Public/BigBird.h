// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bird.h"
#include "BigBird.generated.h"

/**
 * 
 */
UCLASS()
class LEARN_API ABigBird : public ABird
{
	GENERATED_BODY()

public:

	UFUNCTION()
		void ScaleBird(int MNum);
	
};
