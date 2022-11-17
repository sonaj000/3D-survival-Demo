// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner/BaseItem.h"
#include "HealthUp.generated.h"

/**
 * 
 */
UCLASS()
class LEARN_API AHealthUp : public ABaseItem
{
	GENERATED_BODY()
public:
	virtual void PowerUp(AActor* MCharacter) override;

};
