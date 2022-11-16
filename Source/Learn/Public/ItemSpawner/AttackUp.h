// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner/BaseItem.h"
#include "AttackUp.generated.h"

/**
 * 
 */
UCLASS()
class LEARN_API AAttackUp : public ABaseItem
{
	GENERATED_BODY()
	
public:
		virtual void PowerUp(AActor* MCharacter) override;
};
