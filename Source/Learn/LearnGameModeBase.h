// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LearnGameModeBase.generated.h"

/**
 * 
 */

class APoints;

UCLASS()
class LEARN_API ALearnGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALearnGameModeBase();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		int TotalScore;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<APoints>PointItem;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void SpawnPoint();

	
};
