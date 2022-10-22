// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlockManager.generated.h"


class ABird;
class ABigBird;

UCLASS()
class LEARN_API AFlockManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockManager();

	TArray<AActor*>AllBirds;

	TMap<int,TArray<AActor*>>Groups;

	UPROPERTY(VisibleAnywhere, Category = "Merge")
	TSubclassOf<ABigBird>NewBB;


	UFUNCTION()
		void Initialize();

	UFUNCTION()
		void MergeFlock(TArray<AActor*>NewFlock);

	UFUNCTION()
		void DestroyFlock(TArray<AActor*>NewFlock, FVector spawnloc);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
