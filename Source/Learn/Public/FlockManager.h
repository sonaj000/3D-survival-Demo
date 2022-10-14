// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlockManager.generated.h"


class ABird;

UCLASS()
class LEARN_API AFlockManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockManager();

	TArray<AActor*>AllBirds;

	TMap<ABird,TArray<ABird*>>Groups;

	UFUNCTION()
		void Initialize();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
