// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "DispenserManager.generated.h"

class ABaseItem;
class ADispenser;

UCLASS()
class LEARN_API ADispenserManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADispenserManager();

	TQueue<ABaseItem*>Items;

	UPROPERTY()
	TArray<AActor*>AllSites;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void ItemCall();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
