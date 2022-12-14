// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner/BaseItem.h"
#include "Points.generated.h"

/**
 * 
 */
UCLASS()
class LEARN_API APoints : public ABaseItem
{
	GENERATED_BODY()
public:
	virtual void PowerUp(AActor* MCharacter) override;

	virtual void BeginOverLap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
